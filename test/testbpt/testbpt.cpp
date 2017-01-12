#include <string>
#include "gtest/gtest.h"
#define UNIT_TEST 0
#include "bpt.h"

using namespace std;

TEST(Bpt, tree_initialize) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10);
    Value a(1);
    EXPECT_EQ(tree->level, 5);
    EXPECT_EQ(tree->entries, 10);
    EXPECT_EQ(tree->order, 10);
}

TEST(Bpt, tree_insert) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10);
    Value a(1);
    tree->bplus_tree_insert(a, 1);
    bplus_non_leaf *root = (bplus_non_leaf *)tree->root;
    EXPECT_EQ(root->key[0].IntValue, 1);
}

TEST(Bpt, tree_search) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10);
    Value a(1);
    tree->bplus_tree_insert(a, 1);
    bplus_non_leaf *root = (bplus_non_leaf *)tree->root;
    EXPECT_EQ(root->key[0].IntValue, 1);
}




