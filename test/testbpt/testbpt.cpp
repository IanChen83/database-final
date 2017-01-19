#include <string>
#include <iostream>
#include "gtest/gtest.h"
#define UNIT_TEST 0
#include "bpt.h"
#include "bpt.cpp"

using namespace std;

TEST(Bpt, tree_initialize) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10, ValueType::Integer);
    Value a(1);
    EXPECT_EQ(tree->m_level, 5);
    EXPECT_EQ(tree->entries, 10);
    EXPECT_EQ(tree->order, 10);
}

TEST(Bpt, int_tree_insert) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10, ValueType::Integer);
    Value a(1);
    EXPECT_TRUE(tree->bplus_tree_insert(a, 1));
    bplus_leaf *root = (bplus_leaf *)tree->root;
    EXPECT_EQ(tree->root, tree->head[0]);
    EXPECT_EQ(root->data[0], 1);
    EXPECT_FALSE(tree->bplus_tree_insert(a, 1));
}


TEST(Bpt, int_tree_delete) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10, ValueType::Integer);
    Value a(1);
    Value b(2);
    EXPECT_TRUE(tree->bplus_tree_insert(a, 1));
    bplus_leaf *root = (bplus_leaf *)tree->root;
    EXPECT_FALSE(tree->bplus_tree_delete(b));
    EXPECT_TRUE(tree->bplus_tree_delete(a));
}

TEST(Bpt, int_tree_search) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10, ValueType::Integer);
    for(int i = 0; i < 20; i++) {
        EXPECT_TRUE(tree->bplus_tree_insert(Value(i), i));
    }
    EXPECT_EQ(tree->bplus_tree_search(Value(1)), 1);
    vector<rid_t> range_search = tree->bplus_tree_get_range(Value(1), Value(4));
    EXPECT_EQ(range_search[0], 1);
    EXPECT_EQ(range_search[1], 2);
    EXPECT_EQ(range_search[2], 3);
    EXPECT_EQ(range_search[3], 4);
    EXPECT_EQ(range_search.size(), 4);
}

TEST(Bpt, int_tree_get_num) {
    bplus_tree *tree = new bplus_tree(10, 3, 3, ValueType::Integer);
    for(int i = 0; i < 10; i++) {
        EXPECT_TRUE(tree->bplus_tree_insert(Value(i), i));
    }
    EXPECT_EQ(tree->get_leaf_num(), 5);
    EXPECT_EQ(tree->get_non_leaf_num(), 3);
    tree->bplus_tree_delete(Value(6));
    tree->bplus_tree_delete(Value(7));
    tree->bplus_tree_delete(Value(8));
    tree->bplus_tree_insert(Value(6), 6);
    tree->bplus_tree_insert(Value(7), 7);
    tree->bplus_tree_insert(Value(8), 8);
    EXPECT_EQ(tree->get_leaf_num(), 5);
    EXPECT_EQ(tree->get_non_leaf_num(), 3);
}

TEST(Bpt, string_tree_insert) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10, ValueType::String);
    Value a("a");
    EXPECT_TRUE(tree->bplus_tree_insert(a, 1));
    bplus_leaf *root = (bplus_leaf *)tree->root;
    EXPECT_EQ(root->key[0].StrValue, "a");
    EXPECT_EQ(root->data[0], 1);
}

TEST(Bpt, string_tree_delete) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10, ValueType::String);
    Value a("a");
    Value b("b");
    EXPECT_TRUE(tree->bplus_tree_insert(a, 1));
    bplus_leaf *root = (bplus_leaf *)tree->root;
    EXPECT_FALSE(tree->bplus_tree_delete(b));
    EXPECT_TRUE(tree->bplus_tree_delete(a));

}

TEST(Bpt, string_tree_search) {
    bplus_tree *tree = new bplus_tree(5, 10 ,10, ValueType::String);
    Value a("asdmslqw");
    Value b("aqjwejqwemw");
    tree->bplus_tree_insert(a, 1);
    tree->bplus_tree_insert(b, 2);
    EXPECT_EQ(tree->bplus_tree_search(a), 1);
    EXPECT_EQ(tree->bplus_tree_search(b), 2);
}

TEST(Bpt, key_binary_search) {
    Value arr[] = {Value("nkpgrtgady"), Value("ksxzilnqvg"), Value("fbajhdwzzv"), Value("sbscccjpui")};
    cout << key_binary_search(arr, 3, Value("dhvldaieus")) << endl;
}

