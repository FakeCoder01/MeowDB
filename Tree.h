/* Tree.h */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#define TagRoot     1
#define TagNode     2
#define TagLeaf     4

#define NoError     0

#ifdef TREE
    typedef void* Zeroptr;
    Zeroptr zeroptr = 0;
#endif

#define find_last(x)        find_last_linear(x)
#define reterr(x) \
    errno = (x); \
    return zeroptr;

typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char int8;
typedef unsigned char Tag;

// Structure representing a node in the tree
struct TreeNode {
    Tag tag; // type of the node (root, node, leaf)
    struct TreeNode *north; // pointer to the parent node
    struct TreeNode *west; // pointer to the left child node
    struct TreeLeaf *east; // pointer to the right child leaf
    int8 path[256]; // path associated with the node
};
typedef struct TreeNode Node;

// structure representing a leaf in the tree
struct TreeLeaf {
    Tag tag; // type of the leaf
    union u_tree *west; // pointer to the left sibling
    struct TreeLeaf *east; // pointer to the right sibling
    int8 key[128]; // key associated with the leaf
    int8 *value; // value stored in the leaf
    int16 size; // size of the value
};
typedef struct TreeLeaf Leaf;

// union representing either a node or a leaf
union u_tree {
    Node n;
    Leaf l;
};
typedef union u_tree Tree;

// function prototypes
void zero(int8*, int16);
Node *create_tree_node(Node*, int8*);
Leaf *find_last_linear(Node*);
Leaf *create_tree_leaf(Node*, int8*, int8*, int16);
Leaf *select_leaf(Node*, int8*);
int32 insert_leaf(Node*, int8*, int8*, int16);
int32 delete_leaf(Node*, int8*);
