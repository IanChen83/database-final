#ifndef BPT_HEADER
#define BPT_HEADER
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "action.h"

#define MIN_ORDER        3
#define MAX_ORDER        64
#define MAX_ENTRIES      64
#define MAX_LEVEL        10
/* the encapulated B+ tree */
int init_test();

typedef struct {
    size_t order; /* `order` of B+ tree */
    size_t value_size; /* size of value */
    size_t key_size;   /* size of key */
    size_t internal_node_num; /* how many internal nodes */
    size_t leaf_node_num;     /* how many leafs */
    size_t height;            /* height of tree (exclude leafs) */
    off_t slot;        /* where to store new block */
    off_t root_offset; /* where is the root of internal nodes */
    off_t leaf_offset; /* where is the first leaf */
} meta_t;

struct bplus_node {
        int type;
        struct bplus_non_leaf *parent;
};

struct bplus_non_leaf {
        int type;
        struct bplus_non_leaf *parent;
        struct bplus_non_leaf *next;
        int children;
        Value key[MAX_ORDER - 1];
        struct bplus_node *sub_ptr[MAX_ORDER];
};

struct bplus_leaf {
        int type;
        struct bplus_non_leaf *parent;
        struct bplus_leaf *next;
        int entries;
        Record record[MAX_ENTRIES];
};

class bplus_tree {
public:
    void bplus_tree_dump();
    int bplus_tree_get(Value key);
    int bplus_tree_put(Value key, char* data);
    int bplus_tree_get_range(Value key1, Value key2);
    struct bplus_tree *bplus_tree_init(int level, int order, int entries);
    void bplus_tree_deinit();


#ifndef UNIT_TEST
private:
#else
public:
#endif
    meta_t meta;
    int order;
    int entries;
    int level;
    struct bplus_node *root;
    struct bplus_node *head[MAX_LEVEL];
};
#endif
