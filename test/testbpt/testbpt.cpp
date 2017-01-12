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

TEST(Bpt, int_tree_insert) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10);
    Value a(1);
    EXPECT_TRUE(tree->bplus_tree_insert(a, 1));
    bplus_leaf *root = (bplus_leaf *)tree->root;
    EXPECT_EQ(tree->root, tree->head[0]);
    EXPECT_EQ(root->data[0], 1);

}

TEST(Bpt, int_tree_search) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10);
    Value a(1);
    tree->bplus_tree_insert(a, 1);
    bplus_leaf *root = (bplus_leaf *)tree->root;
    EXPECT_EQ(root->key[0].IntValue, 1);
    EXPECT_EQ(tree->bplus_tree_search(a), 1);
}

TEST(Bpt, string_tree_insert) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10);
    Value a("a");
    EXPECT_TRUE(tree->bplus_tree_insert(a, 1));
    bplus_leaf *root = (bplus_leaf *)tree->root;
    EXPECT_EQ(root->key[0].StrValue, "a");
    EXPECT_EQ(root->data[0], 1);
}

TEST(Bpt, string_tree_search) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10);
    Value a("a");
    tree->bplus_tree_insert(a, 1);
    bplus_leaf *root = (bplus_leaf *)tree->root;  
    EXPECT_EQ(tree->bplus_tree_search(a), 1);
}



