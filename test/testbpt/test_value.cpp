#include "gtest/gtest.h"
#include "value.h"
#include <iostream>

TEST(Value, IntOperator) {
    Value* a = createValue(2);
    Value* b = createValue(0);
    Value* c = createValue(0);
    ASSERT_TRUE(a != NULL);
    EXPECT_TRUE(*a > *b);
    EXPECT_FALSE(*b > *a);
    EXPECT_FALSE(*a < *b);
    EXPECT_TRUE(*b < *a);
    EXPECT_TRUE(*a != *b);
    EXPECT_FALSE(*b != *c);
}

TEST(Value, StrOperator) {
    Value* a = createValue("abcde");
    Value* b = createValue("aaaa");
    Value* c = createValue("aaaa");
    ASSERT_TRUE(a != NULL);
    EXPECT_TRUE(*a > *b);
    EXPECT_FALSE(*b > *a);
    EXPECT_FALSE(*a < *b);
    EXPECT_TRUE(*b < *a);
    EXPECT_TRUE(*a != *b);
    EXPECT_FALSE(*c != *b);    
}