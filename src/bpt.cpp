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

void
bplus_tree::non_leaf_remove(bplus_non_leaf *node, int remove, int level)
{
    int i, j, k;
    bplus_non_leaf *sibling;

    if (node->children <= (order + 1) / 2) {
        bplus_non_leaf *parent = (bplus_non_leaf *)node->parent;
        if (parent != NULL) {
            int borrow = 0;
            /* find which sibling node with same parent to be borrowed from */
            i = key_binary_search(parent->key, parent->children - 1, node->key[0]);
            assert(i < 0);
            i = -i - 1;
            if (i == 0) {
                /* no left sibling, choose right one */
                sibling = (bplus_non_leaf *)parent->sub_ptr[i + 1];
                borrow = BORROW_FROM_RIGHT;
            } else if (i == parent->children - 1) {
                /* no right sibling, choose left one */
                sibling = (bplus_non_leaf *)parent->sub_ptr[i - 1];
                borrow = BORROW_FROM_LEFT;
            } else {
                bplus_non_leaf *l_sib = (bplus_non_leaf *)parent->sub_ptr[i - 1];
                bplus_non_leaf *r_sib = (bplus_non_leaf *)parent->sub_ptr[i + 1];
                /* if both left and right sibling found, choose the one with more children */
                sibling = l_sib->children >= r_sib->children ? l_sib : r_sib;
                borrow = l_sib->children >= r_sib->children ? BORROW_FROM_LEFT : BORROW_FROM_RIGHT;
            }

            /* locate parent node key to update later */
            i = i - 1;

            if (borrow == BORROW_FROM_LEFT) {
                if (sibling->children > (order + 1) / 2) {
                    /* node's elements right shift */
                    for (j = remove; j > 0; j--) {
                        node->key[j] = node->key[j - 1];
                    }
                    for (j = remove + 1; j > 0; j--) {
                        node->sub_ptr[j] = node->sub_ptr[j - 1];
                    }
                    /* parent key right rotation */
                    node->key[0] = parent->key[i];
                    parent->key[i] = sibling->key[sibling->children - 2];
                    /* borrow the last sub-node from left sibling */
                    node->sub_ptr[0] = sibling->sub_ptr[sibling->children - 1];
                    sibling->sub_ptr[sibling->children - 1]->parent = node;
                    sibling->children--;
                } else {
                    /* move parent key down */
                    sibling->key[sibling->children - 1] = parent->key[i];
                    /* merge with left sibling */
                    for (j = sibling->children, k = 0; k < node->children - 1; k++) {
                        if (k != remove) {
                            sibling->key[j] = node->key[k];
                            j++;
                        }
                    }
                    for (j = sibling->children, k = 0; k < node->children; k++) {
                        if (k != remove + 1) {
                            sibling->sub_ptr[j] = node->sub_ptr[k];
                            node->sub_ptr[k]->parent = sibling;
                            j++;
                        }
                    }
                    sibling->children = j;
                    /* delete merged node */
                    sibling->next = node->next;
                    non_leaf_delete(node);
                    /* trace upwards */
                    non_leaf_remove(parent, i, level + 1);
                }
            } else {
                /* remove key first in case of overflow during merging with sibling node */
                for (; remove < node->children - 2; remove++) {
                    node->key[remove] = node->key[remove + 1];
                    node->sub_ptr[remove + 1] = node->sub_ptr[remove + 2];
                }
                node->children--;
                if (sibling->children > (order + 1) / 2) {
                    /* parent key left rotation */
                    node->key[node->children - 1] = parent->key[i + 1];
                    parent->key[i + 1] = sibling->key[0];
                    /* borrow the frist sub-node from right sibling */
                    node->sub_ptr[node->children] = sibling->sub_ptr[0];
                    sibling->sub_ptr[0]->parent = node;
                    node->children++;
                    /* left shift in right sibling */
                    for (j = 0; j < sibling->children - 2; j++) {
                        sibling->key[j] = sibling->key[j + 1];
                    }
                    for (j = 0; j < sibling->children - 1; j++) {
                        sibling->sub_ptr[j] = sibling->sub_ptr[j + 1];
                    }
                    sibling->children--;
                } else {
                    /* move parent key down */
                    node->key[node->children - 1] = parent->key[i + 1];
                    node->children++;
                    /* merge with right sibling */
                    for (j = node->children - 1, k = 0; k < sibling->children - 1; j++, k++) {
                        node->key[j] = sibling->key[k];
                    }
                    for (j = node->children - 1, k = 0; k < sibling->children; j++, k++) {
                        node->sub_ptr[j] = sibling->sub_ptr[k];
                        sibling->sub_ptr[k]->parent = node;
                    }
                    node->children = j;
                    /* delete merged sibling */
                    node->next = sibling->next;
                    non_leaf_delete(sibling);
                    /* trace upwards */
                    non_leaf_remove(parent, i + 1, level + 1);
                }
            }
            /* deletion finishes */
            return;
        } else {
            if (node->children == 2) {
                /* delete old root node */
                assert(remove == 0);
                node->sub_ptr[0]->parent = NULL;
                root = node->sub_ptr[0];
                head[level] = NULL;
                non_leaf_delete(node);
                return;
            }
        }
    }
    
    /* simple deletion */
    assert(node->children > 2);
    for (; remove < node->children - 2; remove++) {
        node->key[remove] = node->key[remove + 1];
        node->sub_ptr[remove + 1] = node->sub_ptr[remove + 2];
    }
    node->children--;
}

bool
bplus_tree::leaf_remove(bplus_leaf *leaf, Value key)
{
    int i, j, k;
    bplus_leaf *sibling;

    int remove = key_binary_search(leaf->key, leaf->entries, key);
    if (remove < 0) {
        /* Not exist */
        return false;
    }

    if (leaf->entries <= (entries + 1) / 2) {
        bplus_non_leaf *parent = (bplus_non_leaf *)leaf->parent;
        if (parent != NULL) {
            int borrow = 0;
            /* find which sibling node with same parent to be borrowed from */
            i = key_binary_search(parent->key, parent->children - 1, leaf->key[0]);
            if (i >= 0) {
                i = i + 1;
                if (i == parent->children - 1) {
                    /* the last node, no right sibling, choose left one */
                    sibling = (bplus_leaf *)parent->sub_ptr[i - 1];
                    borrow = BORROW_FROM_LEFT;
                } else {
                    bplus_leaf *l_sib = (bplus_leaf *)parent->sub_ptr[i - 1];
                    bplus_leaf *r_sib = (bplus_leaf *)parent->sub_ptr[i + 1];
                    /* if both left and right sibling found, choose the one with more entries */
                    sibling = l_sib->entries >= r_sib->entries ? l_sib : r_sib;
                    borrow = l_sib->entries >= r_sib->entries ? BORROW_FROM_LEFT : BORROW_FROM_RIGHT;
                }
            } else {
                i = -i - 1;
                if (i == 0) {
                    /* the frist node, no left sibling, choose right one */
                    sibling = (bplus_leaf *)parent->sub_ptr[i + 1];
                    borrow = BORROW_FROM_RIGHT;
                } else if (i == parent->children - 1) {
                    /* the last node, no right sibling, choose left one */
                    sibling = (bplus_leaf *)parent->sub_ptr[i - 1];
                    borrow = BORROW_FROM_LEFT;
                } else {
                    bplus_leaf *l_sib = (bplus_leaf *)parent->sub_ptr[i - 1];
                    bplus_leaf *r_sib = (bplus_leaf *)parent->sub_ptr[i + 1];
                    /* if both left and right sibling found, choose the one with more entries */
                    sibling = l_sib->entries >= r_sib->entries ? l_sib : r_sib;
                    borrow = l_sib->entries >= r_sib->entries ? BORROW_FROM_LEFT : BORROW_FROM_RIGHT;
                }
            }

            /* locate parent node key to update later */
            i = i - 1;

            if (borrow == BORROW_FROM_LEFT) {
                if (sibling->entries > (entries + 1) / 2) {
                    /* right shift in leaf node */
                    for (; remove > 0; remove--) {
                        leaf->key[remove] = leaf->key[remove - 1];
                        leaf->data[remove] = leaf->data[remove - 1];
                    }
                    /* borrow the last element from left sibling */
                    leaf->key[0] = sibling->key[sibling->entries - 1];
                    leaf->data[0] = sibling->data[sibling->entries - 1];
                    sibling->entries--;
                    /* update parent key */
                    parent->key[i] = leaf->key[0];
                } else {
                    /* merge with left sibling */
                    for (j = sibling->entries, k = 0; k < leaf->entries; k++) {
                        if (k != remove) {
                            sibling->key[j] = leaf->key[k];
                            sibling->data[j] = leaf->data[k];
                            j++;
                        }
                    }
                    sibling->entries = j;
                    /* delete merged leaf */
                    sibling->next = leaf->next;
                    leaf_delete(leaf);
                    /* trace upwards */
                    non_leaf_remove(parent, i, 1);
                }
            } else {
                /* remove element first in case of overflow during merging with sibling node */
                for (; remove < leaf->entries - 1; remove++) {
                    leaf->key[remove] = leaf->key[remove + 1];
                    leaf->data[remove] = leaf->data[remove + 1];
                }
                leaf->entries--;
                if (sibling->entries > (entries + 1) / 2) {
                    /* borrow the first element from right sibling */
                    leaf->key[leaf->entries] = sibling->key[0];
                    leaf->data[leaf->entries] = sibling->data[0];
                    leaf->entries++;
                    /* left shift in right sibling */
                    for (j = 0; j < sibling->entries - 1; j++) {
                        sibling->key[j] = sibling->key[j + 1];
                        sibling->data[j] = sibling->data[j + 1];
                    }
                    sibling->entries--;
                    /* update parent key */
                    parent->key[i + 1] = sibling->key[0];
                } else {
                    /* merge with right sibling */
                    for (j = leaf->entries, k = 0; k < sibling->entries; j++, k++) {
                        leaf->key[j] = sibling->key[k];
                        leaf->data[j] = sibling->data[k];
                    }
                    leaf->entries = j;
                    /* delete right sibling */
                    leaf->next = sibling->next;
                    leaf_delete(sibling);
                    /* trace upwards */
                    non_leaf_remove(parent, i + 1, 1);
                }
            }
            /* deletion finishes */
            return 0;
        } else {
            if (leaf->entries == 1) {
                /* delete the only last node */
                assert(key == leaf->key[0]);
                root = NULL;
                head[0] = NULL;
                leaf_delete(leaf);
                return true;
            }
        }
    }

    /* simple deletion */
    for (; remove < leaf->entries - 1; remove++) {
        leaf->key[remove] = leaf->key[remove + 1];
        leaf->data[remove] = leaf->data[remove + 1];
    }
    leaf->entries--;

    return true;
}

bool
bplus_tree::bplus_tree_delete(Value key)
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
            return leaf_remove(ln, key);
        default:
            assert(0);
        }
    }

    return -1;
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
    bplus_non_leaf *sibling;

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
