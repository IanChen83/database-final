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
bplus_tree::bplus_tree_search(Value key)
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

bplus_tree::bplus_tree(int level, int order, int entries)
	: level(level), order(order), entries(entries), root(NULL) {
        head = new bplus_node[MAX_LEVEL];
}


bplus_tree::~bplus_tree() {	
	delete[] head;
}
