#ifndef BPT_HEADER
#define BPT_HEADER
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include "value.h"

#define MIN_ORDER        3
#define MAX_ORDER        128
#define MAX_ENTRIES      128
#define MAX_LEVEL        10
/* the encapulated B+ tree */

using namespace std;

typedef int32_t rid_t;

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
        bplus_node *parent;
    virtual ~bplus_node() {};
};

struct bplus_non_leaf: bplus_node {
    bplus_non_leaf(): bplus_node() {

    }
        bplus_non_leaf *next;
        int children;
        Value key[MAX_ORDER - 1];
        bplus_node *sub_ptr[MAX_ORDER];
};

struct bplus_leaf: bplus_node {
    bplus_leaf(): bplus_node() {

    }
    ~bplus_leaf() {

    }
        bplus_leaf *next;
        int entries;
        Value key[MAX_ENTRIES];
        rid_t data[MAX_ENTRIES];
};

class bplus_tree {
public:
    bplus_tree(int, int, int, ValueType);
    ~bplus_tree();
    void bplus_tree_dump();
    rid_t bplus_tree_get(Value);
    bool bplus_tree_insert(Value, rid_t);
    rid_t bplus_tree_search(Value key);
    bool bplus_tree_delete(Value);
    vector<rid_t> bplus_tree_get_range(Value, Value);
    //return pair<index, leaf>
    pair<int, int> get_page_content();
    void bplus_tree_deinit();

    // helper
    bplus_leaf* leaf_new();
    bplus_non_leaf* non_leaf_new();
    void non_leaf_delete(bplus_non_leaf*);
    void leaf_delete(bplus_leaf*);

    int get_leaf_num();
    int get_non_leaf_num();


#ifndef UNIT_TEST
private:
#else
public:
#endif
    //// private method
    bool leaf_insert(bplus_leaf *leaf, Value key, rid_t data);
    bool non_leaf_insert(bplus_non_leaf *node, bplus_node *sub_node, Value key, int level);
    void non_leaf_remove(bplus_non_leaf *node, int remove, int level);
    bool leaf_remove(bplus_leaf *leaf, Value key);

    ////
    //// data member
    int order;
    int entries;
    int m_level;
    int non_leaf_num;
    int leaf_num;
    ValueType valueType;
    bplus_node *root;
    bplus_node **head;
};
#endif
