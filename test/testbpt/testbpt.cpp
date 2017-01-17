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
    EXPECT_FALSE(tree->bplus_tree_insert(a, 1));
}


TEST(Bpt, int_tree_delete) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10);
    Value a(1);
    Value b(2);
    EXPECT_TRUE(tree->bplus_tree_insert(a, 1));
    bplus_leaf *root = (bplus_leaf *)tree->root;
    EXPECT_FALSE(tree->bplus_tree_delete(b));
    EXPECT_TRUE(tree->bplus_tree_delete(a));
}

TEST(Bpt, int_tree_search) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10);
    Value a(1);
    Value b(2);
    Value c(3);
    Value d(4);
    Value e(5);
    EXPECT_TRUE(tree->bplus_tree_insert(a, 1));
    EXPECT_TRUE(tree->bplus_tree_insert(b, 2));
    EXPECT_TRUE(tree->bplus_tree_insert(c, 3));
    EXPECT_TRUE(tree->bplus_tree_insert(d, 4));
    EXPECT_TRUE(tree->bplus_tree_insert(e, 5));
    bplus_leaf *root = (bplus_leaf *)tree->root;
    EXPECT_EQ(root->key[0].IntValue, 1);
    EXPECT_EQ(tree->bplus_tree_search(a), 1);
    vector<rid_t> range_search = tree->bplus_tree_get_range(a, d);
    EXPECT_EQ(range_search[0], 1);
    EXPECT_EQ(range_search[1], 2);
    EXPECT_EQ(range_search[2], 3);
    EXPECT_EQ(range_search[3], 4);
    EXPECT_EQ(range_search.size(), 4);
}

TEST(Bpt, string_tree_insert) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10);
    Value a("a");
    EXPECT_TRUE(tree->bplus_tree_insert(a, 1));
    bplus_leaf *root = (bplus_leaf *)tree->root;
    EXPECT_EQ(root->key[0].StrValue, "a");
    EXPECT_EQ(root->data[0], 1);
}

TEST(Bpt, string_tree_delete) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10);
    Value a("a");
    Value b("b");
    EXPECT_TRUE(tree->bplus_tree_insert(a, 1));
    bplus_leaf *root = (bplus_leaf *)tree->root;
    EXPECT_FALSE(tree->bplus_tree_delete(b));
    EXPECT_TRUE(tree->bplus_tree_delete(a));

}

TEST(Bpt, string_tree_search) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10);
    Value a("a");
    tree->bplus_tree_insert(a, 1);
    bplus_leaf *root = (bplus_leaf *)tree->root;  
    EXPECT_EQ(tree->bplus_tree_search(a), 1);
}



