#include "gtest/gtest.h"
#include "action.h"
#include <iostream>

TEST(Value, IntOperator) {
    Value* a = createValue(2);
    Value* b = createValue(0);
    ASSERT_TRUE(a != NULL);
    EXPECT_FALSE(*a < *b);
    EXPECT_TRUE(*b < *a);
}

TEST(Value, StrOperator) {
    Value* a = createValue("abcde");
    Value* b = createValue("aaaa");
    ASSERT_TRUE(a != NULL);
    EXPECT_FALSE(*a < *b);
    EXPECT_TRUE(*b < *a);
}