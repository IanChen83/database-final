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
key_binary_search(int *arr, int len, int target)
{
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

static inline struct bplus_non_leaf *
non_leaf_new(void)
{
	struct bplus_non_leaf *node = new bplus_non_leaf;
	assert(node != NULL);
	node->type = BPLUS_TREE_NON_LEAF;
	return node;
}

static inline struct bplus_leaf *
leaf_new(void)
{
	struct bplus_leaf *node = new bplus_leaf;
	assert(node != NULL);
	node->type = BPLUS_TREE_LEAF;
	return node;
}

static inline void
non_leaf_delete(struct bplus_non_leaf *node)
{
	delete node;
	node = NULL;
}

static inline void
leaf_delete(struct bplus_leaf *node)
{
	delete node;
	node = NULL;
}


int init_test() {
    return 0;
}
