#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "bpt.h"
using namespace std;
enum {
    BPLUS_TREE_LEAF,
    BPLUS_TREE_NON_LEAF = 1,
};

enum {
    BORROW_FROM_LEFT,
    BORROW_FROM_RIGHT = 1,
};

static inline int
key_binary_search(Value *arr, int len, Value target) {
    int low = -1;
    int high = len;
    while (low + 1 < high) {
    int mid = low + (high - low) / 2;
    if (target > arr[mid]) {
        low = mid;
    } else {
        high = mid;
    }
    }
    if (high >= len || arr[high] != target) {
    return -high - 1;
    } else {
    return high;
    }
}

static inline bplus_non_leaf *
non_leaf_new(void) {
    bplus_non_leaf *node = new bplus_non_leaf;
    assert(node != NULL);
    node->type = BPLUS_TREE_NON_LEAF;
    return node;
}

static inline bplus_leaf *
leaf_new(void) {
    bplus_leaf *node = new bplus_leaf;
    assert(node != NULL);
    node->type = BPLUS_TREE_LEAF;
    return node;
}

static inline void
non_leaf_delete(bplus_non_leaf *node) {
    delete node;
    node = NULL;
}

static inline void
leaf_delete(bplus_leaf *node) {
    delete node;
    node = NULL;
}

rid_t
bplus_tree::bplus_tree_search(Value key) {
    int i;
    bplus_node *node = root;
    bplus_non_leaf *nln;
    bplus_leaf *ln;

    while (node != NULL) {
        switch (node->type) {
        case BPLUS_TREE_NON_LEAF:
            nln = (bplus_non_leaf *)node;
            i = key_binary_search(nln->key, nln->children - 1, key);
            if (i >= 0) {
                node = nln->sub_ptr[i + 1];
            } else {
                i = -i - 1;
                node = nln->sub_ptr[i];
            }
            break;
        case BPLUS_TREE_LEAF:
            ln = (bplus_leaf *)node;
            i = key_binary_search(ln->key, ln->entries, key);
            if (i >= 0) {
                return ln->data[i];
            } else {
                return 0;
            }
        default:
            assert(0);
        }
    }

    return 0;
}

bool
bplus_tree::non_leaf_insert(bplus_non_leaf *node, bplus_node *sub_node, Value key, int level)
{
        int i, j;
        Value split_key;
        int split = 0;
        struct bplus_non_leaf *sibling;

        int insert = key_binary_search(node->key, node->children - 1, key);
        assert(insert < 0);
        insert = -insert - 1;

        /* node full */
        if (node->children == order) {
                /* split = [m/2] */
                split = (order + 1) / 2;
                /* splited sibling node */
                sibling = non_leaf_new();
                sibling->next = node->next;
                node->next = sibling;
                /* non-leaf node's children always equals to split + 1 after insertion */
                node->children = split + 1;
                /* sibling node replication due to location of insertion */
                if (insert < split) {
                        split_key = node->key[split - 1];
                        /* sibling node's first sub-node */
                        sibling->sub_ptr[0] = node->sub_ptr[split];
                        node->sub_ptr[split]->parent = sibling;
                        /* insertion point is before split point, replicate from key[split] */
                        for (i = split, j = 0; i < order - 1; i++, j++) {
                                sibling->key[j] = node->key[i];
                                sibling->sub_ptr[j + 1] = node->sub_ptr[i + 1];
                                node->sub_ptr[i + 1]->parent = sibling;
                        }
                        sibling->children = j + 1;
                        /* insert new key and sub-node */
                        for (i = node->children - 2; i > insert; i--) {
                                node->key[i] = node->key[i - 1];
                                node->sub_ptr[i + 1] = node->sub_ptr[i];
                        }
                        node->key[i] = key;
                        node->sub_ptr[i + 1] = sub_node;
                        sub_node->parent = node;
                } else if (insert == split) {
                        split_key = key;
                        /* sibling node's first sub-node */
                        sibling->sub_ptr[0] = sub_node;
                        sub_node->parent = sibling;
                        /* insertion point is split point, replicate from key[split] */
                        for (i = split, j = 0; i < order - 1; i++, j++) {
                                sibling->key[j] = node->key[i];
                                sibling->sub_ptr[j + 1] = node->sub_ptr[i + 1];
                                node->sub_ptr[i + 1]->parent = sibling;
                        }
                        sibling->children = j + 1;
                } else {
                        split_key = node->key[split];
                        /* sibling node's first sub-node */
                        sibling->sub_ptr[0] = node->sub_ptr[split + 1];
                        node->sub_ptr[split + 1]->parent = sibling;
                        /* insertion point is after split point, replicate from key[split + 1] */
                        for (i = split + 1, j = 0; i < order - 1; j++) {
                                if (j != insert - split - 1) {
                                        sibling->key[j] = node->key[i];
                                        sibling->sub_ptr[j + 1] = node->sub_ptr[i + 1];
                                        node->sub_ptr[i + 1]->parent = sibling;
                                        i++;
                                }
                        }
                        /* reserve a hole for insertion */
                        if (j > insert - split - 1) {
                                sibling->children = j + 1;
                        } else {
                                assert(j == insert - split - 1);
                                sibling->children = j + 2;
                        }
                        /* insert new key and sub-node*/
                        j = insert - split - 1;
                        sibling->key[j] = key;
                        sibling->sub_ptr[j + 1] = sub_node;
                        sub_node->parent = sibling;
                }
        } else {
                /* simple insertion */
                for (i = node->children - 1; i > insert; i--) {
                        node->key[i] = node->key[i - 1];
                        node->sub_ptr[i + 1] = node->sub_ptr[i];
                }
                node->key[i] = key;
                node->sub_ptr[i + 1] = sub_node;
                node->children++;
        }

        if (split) {
                bplus_non_leaf *parent = (bplus_non_leaf*)node->parent;
                if (parent == NULL) {
                        if (++level >= level) {
                                fprintf(stderr, "!!Panic: Level exceeded, please expand the tree level, non-leaf order or leaf entries for element capacity!\n");
                                node->next = sibling->next;
                                non_leaf_delete(sibling);
                                return false;
                        }
                        /* new parent */
                        parent = non_leaf_new();
                        parent->key[0] = split_key;
                        parent->sub_ptr[0] = node;
                        parent->sub_ptr[1] = sibling;
                        parent->children = 2;
                        /* update root */
                        root = parent;
                        head[level] = parent;
                        node->parent = parent;
                        sibling->parent = parent;
                } else {
                        /* Trace upwards */
                        sibling->parent = parent;
                        return non_leaf_insert(parent, sibling, split_key, level + 1);
                }
        }

        return true;
}


bool
bplus_tree::leaf_insert(bplus_leaf *leaf, Value key, rid_t data)
{
        int i, j, split = 0;
        bplus_leaf *sibling;

        int insert = key_binary_search(leaf->key, leaf->entries, key);
        if (insert >= 0) {
                /* Already exists */
                return false;
        }
        insert = -insert - 1;

        /* node full */
        if (leaf->entries == entries) {
                /* split = [m/2] */
                split = (entries + 1) / 2;
                /* splited sibling node */
                sibling = leaf_new();
                sibling->next = leaf->next;
                leaf->next = sibling;
                /* leaf node's entries always equals to split after insertion */
                leaf->entries = split;
                /* sibling leaf replication due to location of insertion */
                if (insert < split) {
                        /* insertion point is before split point, replicate from key[split - 1] */
                        for (i = split - 1, j = 0; i < entries; i++, j++) {
                                sibling->key[j] = leaf->key[i];
                                sibling->data[j] = leaf->data[i];
                        }
                        sibling->entries = j;
                        /* insert new key and sub-node */
                        for (i = split - 1; i > insert; i--) {
                                leaf->key[i] = leaf->key[i - 1];
                                leaf->data[i] = leaf->data[i - 1];
                        }
                        leaf->key[i] = key;
                        leaf->data[i] = data;
                } else {
                        /* insertion point is or after split point, replicate from key[split] */
                        for (i = split, j = 0; i < entries; j++) {
                                if (j != insert - split) {
                                        sibling->key[j] = leaf->key[i];
                                        sibling->data[j] = leaf->data[i];
                                        i++;
                                }
                        }
                        /* reserve a hole for insertion */
                        if (j > insert - split) {
                                sibling->entries = j;
                        } else {
                                assert(j == insert - split);
                                sibling->entries = j + 1;
                        }
                        /* insert new key */
                        j = insert - split;
                        sibling->key[j] = key;
                        sibling->data[j] = data;
                }
        } else {
                /* simple insertion */
                for (i = leaf->entries; i > insert; i--) {
                        leaf->key[i] = leaf->key[i - 1];
                        leaf->data[i] = leaf->data[i - 1];
                }
                leaf->key[i] = key;
                leaf->data[i] = data;
                leaf->entries++;
        }

        if (split) {
                bplus_non_leaf *parent = (bplus_non_leaf*)leaf->parent;
                if (parent == NULL) {
                        /* new parent */
                        parent = non_leaf_new();
                        parent->key[0] = sibling->key[0];
                        parent->sub_ptr[0] = (bplus_node *)leaf;
                        parent->sub_ptr[1] = (bplus_node *)sibling;
                        parent->children = 2;
                        /* update root */
                        root = parent;
                        head[1] = parent;
                        leaf->parent = parent;
                        sibling->parent = parent;
                } else {
                        /* trace upwards */
                        sibling->parent = parent;
                        return non_leaf_insert(parent, sibling, sibling->key[0], 1);
                }
        }

        return 0;
}


bool
bplus_tree::bplus_tree_insert(Value key, rid_t data)
{
    int i;
    bplus_node *node = root;
    bplus_non_leaf *nln;
    bplus_leaf *ln;

    while (node != NULL) {
        switch (node->type) {
        case BPLUS_TREE_NON_LEAF:
            nln = (bplus_non_leaf *)node;
            i = key_binary_search(nln->key, nln->children - 1, key);
            if (i >= 0) {
                node = nln->sub_ptr[i + 1];
            } else {
                i = -i - 1;
                node = nln->sub_ptr[i];
            }
            break;
        case BPLUS_TREE_LEAF:
            ln = (bplus_leaf *)node;
            return leaf_insert(ln, key, data);
        default:
            return false;
        }
    }

    /* new root */
    root = leaf_new();
    ((bplus_leaf*)root)->key[0] = key;
    ((bplus_leaf*)root)->data[0] = data;
    ((bplus_leaf*)root)->entries = 1;
    head[0] = root;
    return true;
}

bplus_tree::bplus_tree(int level, int order, int entries)
    : level(level), order(order), entries(entries), root(NULL) {
}


bplus_tree::~bplus_tree() { 
}
