/* Tree.c */
#define TREE
#include "Tree.h"

// initializes the root of the tree
Tree root = { .n = {
    .tag = (TagRoot | TagNode),
    .north = (Node *)&root,
    .west = 0,
    .east = 0,
    .path = "/"
}};

// function to zero out a string of specified size
void zero(int8 *str, int16 size) {
    int8 *p;
    int16 n;

    for (n=0, p=str; n < size; p++, n++)
        *p = 0;

    return;
}

// creates a new tree node
Node *create_tree_node(Node *parent, int8 *path) {
    Node *n;
    int16 size;

    errno = NoError;
    assert(parent);
    size = sizeof(struct TreeNode);
    n = (Node *)malloc((int)size);
    zero((int8 *)n, size);
    
    parent->west = n;
    n->tag = TagNode;
    n->north = parent;
    strncpy((char *)n->path, (char *)path, 255);

    return n;
}

// finds the last leaf in a linear fashion
Leaf *find_last_linear(Node *parent) {
    Leaf *l;

    errno = NoError;
    assert(parent);

    if (!parent->east)
        reterr(NoError);

    for (l = parent->east; l->east; l = l->east);
    assert(l);

    return l;
}

// creates a new tree leaf
Leaf *create_tree_leaf(Node *parent, int8 *key, int8 *value, int16 count) {
    Leaf *l, *new_leaf;
    int16 size;
                        
    assert(parent);
    l = find_last(parent);

    size = sizeof(struct TreeLeaf);
    new_leaf = (Leaf *)malloc(size);
    assert(new_leaf);

    if (!l)
        parent->east = new_leaf;
    else
        l->east = new_leaf;

    zero((int8 *)new_leaf, size);
    new_leaf->tag = TagLeaf;
    new_leaf->west = (!l) ? (Tree *)parent : (Tree *)l;

    strncpy((char *)new_leaf->key, (char *)key, 127);
    new_leaf->value = (int8 *)malloc(count);
    zero(new_leaf->value, count);
    assert(new_leaf->value);
    strncpy((char *)new_leaf->value, (char *)value, count);
    new_leaf->size = count;

    return new_leaf;
}

// selects a leaf by key
Leaf *select_leaf(Node *parent, int8 *key) {
    Leaf *current = parent->east; // Start from the first leaf

    while (current) {
        if (strcmp((char *)current->key, (char *)key) == 0) {
            return current; // Return the leaf if the key matches
        }
        current = current->east; // Move to the next leaf
    }
    return NULL; // Return NULL if the key is not found
}

// insert or update a leaf
int32 insert_leaf(Node *parent, int8 *key, int8 *value, int16 count) {
    Leaf *existing_leaf = select_leaf(parent, key);
    if (existing_leaf) {  
        // update existing leaf
        free(existing_leaf->value); // Free the old value
        existing_leaf->value = (int8 *)malloc(count);
        zero(existing_leaf->value, count);
        strncpy((char *)existing_leaf->value, (char *)value, count);
        existing_leaf->size = count;
        return NoError; // return success
    } 
    else {
        // create a new leaf
        create_tree_leaf(parent, key, value, count);
        return NoError; // Return success
    }
}

// delete a leaf by key
int32 delete_leaf(Node *parent, int8 *key) {
    Leaf *current = parent->east;
    Leaf *previous = NULL;

    while (current) {
        if (strcmp((char *)current->key, (char *)key) == 0) {
            if (previous) {
                previous->east = current->east; // bypass the current leaf
            } else {
                parent->east = current->east; // update the first leaf
            }
            free(current->value);
            free(current); // Free the leaf
            return NoError; // return success
        }
        previous = current;
        current = current->east; // move to the next leaf
    }
    return -1; // return error if the key is not found
}
