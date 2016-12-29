#include "gtest/gtest.h"
#include "bpt.h"

TEST(InitTest, InitTest1){
    ASSERT_EQ(init_test(), 0) << "Init test 1";
}

TEST(InitTest2, InitTest2){
    ASSERT_EQ(init_test(), 1) << "Don't worry! This test is expected to failed.";
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
