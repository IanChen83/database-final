#include "gtest/gtest.h"
#include "action.h"

TEST(ActionUsageTest, InalidNullInput) {
    EXPECT_FALSE(isValidInput(NULL));
}

TEST(ActionUsageTest, InvalidRInput) {
    EXPECT_FALSE(isValidInput("R, Student, String, 0"));
    EXPECT_FALSE(isValidInput("R, Student, Integer, 0"));
    EXPECT_FALSE(isValidInput("R, Student, Integer, \"0\""));
    EXPECT_FALSE(isValidInput("R, Student, String,"));
}

TEST(ActionUsageTest, ValidRInput) {
    EXPECT_TRUE(isValidInput("R, Student, integer, 80"));
    EXPECT_TRUE(isValidInput("R, Student, Integer, 80"));
    EXPECT_TRUE(isValidInput("R, Student, string, 80"));
    EXPECT_TRUE(isValidInput("R, Student, String, 80"));
}

TEST(ActionUsageTest, InvalidIInput) {
    EXPECT_FALSE(isValidInput("I, Student,, \"80\""));
    EXPECT_FALSE(isValidInput("I, Student;"));
    EXPECT_FALSE(isValidInput("I, Student, \"string\", \"asdf\"; 39"));
    EXPECT_FALSE(isValidInput("I, Student, \"string\", \"asdf\"; 39, "));
}

TEST(ActionUsageTest, InvalidDInput) {
    EXPECT_FALSE(isValidInput("D, Student, 10"));
    EXPECT_FALSE(isValidInput("D, Student,"));
}

TEST(ActionUsageTest, ValidDInput) {
    EXPECT_TRUE(isValidInput("D, Student, \"10\""));
}
