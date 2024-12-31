#undef TREE
#include "Cache.h"

extern Node root;

// handles the "hello" command
int32 handle_hello(Client *cli, int8 *key, int8 *value) {
    dprintf(cli->socket, "Hello from the Cache server!\n");
    return NoError;
}

bool server_continuation;
bool client_continuation;

// array of command handlers
CmdHandler command_handlers[] = {
    { (int8 *)"hello", handle_hello },
    { (int8 *)"create", handle_create },
    { (int8 *)"insert", handle_insert },
    { (int8 *)"select", handle_select },
    { (int8 *)"update", handle_update },
    { (int8 *)"delete", handle_delete }
};

// get the command handler based on the command string
CommandCallback get_command_handler(int8 *cmd) {
    CommandCallback cb;
    int16 n, arrlen;

    if (sizeof(command_handlers) < 16) return 0;
    arrlen = (sizeof(command_handlers) / sizeof(CmdHandler));

    cb = 0;
    for (n = 0; n < arrlen; n++)
        if (!strcmp((char *)cmd, (char *)command_handlers[n].command)) {
            cb = command_handlers[n].handler;
            break;
        }

    return cb;
}

// handles the "create" command
int32 handle_create(Client *cli, int8 *key, int8 *value) {
    return insert_leaf(&root, key, value, strlen((char *)value)); // create a new key-value pair
}

// handlesthe "insert" command
int32 handle_insert(Client *cli, int8 *key, int8 *value) {
    return insert_leaf(&root, key, value, strlen((char *)value)); // insert or update a key-value pair
}

// handles the "select" command
int32 handle_select(Client *cli, int8 *key, int8 *value) {
    Leaf *leaf = select_leaf(&root, key);
    if (leaf) {
        dprintf(cli->socket, "Value for '%s': %s\n", key, leaf->value);
        return NoError;
    } else {
        dprintf(cli->socket, "Key '%s' not found.\n", key);
        return -1;
    }
}

// handles the "update" command
int32 handle_update(Client *cli, int8 *key, int8 *value) {
    return insert_leaf(&root, key, value, strlen((char *)value)); // update the value for an existing key
}

// handles the "delete" command
int32 handle_delete(Client *cli, int8 *key, int8 *value) {
    return delete_leaf(&root, key); // delete the key-value pair
}

// handles client communication in a loop
void child_loop(Client *cli) {
    int8 buffer[256];
    int16 n;
    int8 *p, *f;
    int8 cmd[256], key[256], value[256];
    CommandCallback cb;

    write(cli->socket, "\nhey ::> ", 9);

    zero(buffer, 256);
    read(cli->socket, (char *)buffer, 255);
    n = (int16)strlen((char *)buffer);
    if (n > 254)
        n = 254;

    for (p = buffer; (*p) && (n--) && (*p != ' ') && (*p != '\n') && (*p != '\r'); p++);

    zero(cmd, 256); zero(key, 256); zero(value, 256);

    if (!(*p) || (!n)) {
        strncpy((char *)cmd, (char *)buffer, 255);
        goto done;
    } else if ((*p == '\n') || (*p == '\r')) {
        *p = 0;
        strncpy((char *)cmd, (char *)buffer, 255);
        goto done;
    } else if (*p == ' ') {
        *p = 0;
        strncpy((char *)cmd, (char *)buffer, 255);
    }

    for (p++, f = p; (*p) && (n--) && (*p != ' ') && (*p != '\n') && (*p != '\r'); p++);

    if (!(*p) || (!n)) {
        strncpy((char *)key, (char *)f, 255);
        goto done;
    } else if ((*p == ' ') || (*p == '\n') || (*p == '\r')) {
        *p = 0;
        strncpy((char *)key, (char *)f, 255);
    }

    p++;
    if (*p) {
        strncpy((char *)value, (char *)p, 255);
        for (p = value; (*p) && (*p != '\n') && (*p != '\r'); p++);
        *p = 0;
    }

done:
    dprintf(cli->socket, "key:\t%s\n", key);
    dprintf(cli->socket, "value:\t%s\n", value);
    
    cb = get_command_handler(cmd);
    if (!cb) {
        dprintf(cli->socket, "400 Command not found: %s\n", cmd);
        return;
    } else {
        cb(cli, key, value);
        return;
    }

    return;
}

// function to handle the main server loop
void main_loop(int s) {
    struct sockaddr_in cli;
    int32 len;
    int s2;
    char *ip;
    int16 port;
    Client *client;
    pid_t pid;

    s2 = accept(s, (struct sockaddr *)&cli, (unsigned int *)&len);
    if (s2 < 0) return;

    port = (int16)htons((int)cli.sin_port);
    ip = inet_ntoa(cli.sin_addr);

    printf("Connection from %s:%d\n", ip, port);

    client = (Client *)malloc(sizeof(struct ClientConnection));
    assert(client);

    zero((int8 *)client, sizeof(struct ClientConnection));
    client->socket = s2;
    client->port = port;
    strncpy(client->ip, ip, 15);

    pid = fork();
    if (pid) {
        free(client);
        return;
    } else {
        dprintf(s2, "Success:: Connected to Cache server\n");

        client_continuation = true;
        while (client_continuation) child_loop(client);

        close(s2);
        free(client);

        return;
    }
    return;
}

// initialize the server
int initialize_server(int16 port) {
    struct sockaddr_in sock;
    int s;

    sock.sin_family = AF_INET;
    sock.sin_port = htons((int)port);
    sock.sin_addr.s_addr = inet_addr(HOST);

    s = socket(AF_INET, SOCK_STREAM, 0); 
    assert(s > 0);

    errno = 0;
    if (bind(s, (struct sockaddr *)&sock, sizeof(sock))) assert_perror(errno);

    errno = 0;
    if (listen(s, 20)) assert_perror(errno);

    printf("Server listening on %s:%d\n", HOST, port);

    return s;
}

// main function to start the server
int main(int argc, char *argv[]) {
    char *sport;
    int16 port;
    int s;

    if (argc < 2)
        sport = PORT;
    else
        sport = argv[1];

    port = (int16)atoi(sport);
    s = initialize_server(port);

    server_continuation = true;
    while (server_continuation)
        main_loop(s);
    
    printf("Shutting down...\n");
    close(s);

    return 0;
}
