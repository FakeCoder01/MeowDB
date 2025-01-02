#ifndef CACHE
#define CACHE

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Tree.h"

#pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic push

#define HOST        "127.0.0.1"
#define PORT        "6969"

// type definitions
typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char int8;

// Structure representing a client connection
struct ClientConnection {
    int socket; // socket descriptor for the client
    char ip[16]; // ip address of the client
    int16 port; // port number of the client
};
typedef struct ClientConnection Client;

// callback function type for command handlers
typedef int32 (*CommandCallback)(Client*, int8*, int8*);

// structure representing a command handler
struct CommandHandler {
    int8* command; // command string
    CommandCallback handler; // function pointer to the command handler
};
typedef struct CommandHandler CmdHandler;

// function prototypes
void child_loop(Client*);
void main_loop(int);
int initialize_server(int16);
int main(int, char**);
int32 handle_create(Client*, int8*, int8*); // handles create command
int32 handle_insert(Client*, int8*, int8*); // handles insert command
int32 handle_select(Client*, int8*, int8*); // handles select command
int32 handle_update(Client*, int8*, int8*); // handles update command
int32 handle_delete(Client*, int8*, int8*); // handles delete command

#endif