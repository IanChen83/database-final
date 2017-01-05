#include "gtest/gtest.h"
#include <iostream>
#include "action.h"

using namespace std;

TEST(ActionUsageTest, createValueFunction) {
    auto x1 = createValue(1);
    ASSERT_FALSE(x1 == NULL);
    EXPECT_EQ(x1->type, ValueType::Integer);
    EXPECT_EQ(x1->IntValue, 1);

    auto x2 = createValue("Value");
    ASSERT_FALSE(x2 == NULL);
    EXPECT_EQ(x2->type, ValueType::String);
    EXPECT_STREQ(x2->StrValue, "Value");

    delete x1;
    delete x2;
}

TEST(ActionUsageTest, InvalidRecord) {
    EXPECT_FALSE(isValidRecord(NULL));
    EXPECT_FALSE(isValidRecord("No double quotes"));
    EXPECT_FALSE(isValidRecord("\"\""));    // Empty record failed?
}

TEST(ActionUsageTest, ValidRecord) {
    EXPECT_TRUE(isValidRecord("\"Normal record\""));
    EXPECT_TRUE(isValidRecord("\"Normal record with spaces   \""));
    EXPECT_TRUE(isValidRecord("\"   \""));  // Space-filled record is valid?
}

TEST(ActionUsageTest, InvalidValue) {
    EXPECT_FALSE(isValidValue(NULL));
    EXPECT_FALSE(isValidValue("a"));
    EXPECT_FALSE(isValidValue("\"a"));
}

TEST(ActionUsageTest, ValidValue) {
    EXPECT_TRUE(isValidValue("10"));
    EXPECT_TRUE(isValidValue("0"));
    EXPECT_TRUE(isValidValue("\"a\""));
    EXPECT_TRUE(isValidValue("\"10\""));
}

TEST(ActionUsageTest, InvalidNullInput) {
    EXPECT_FALSE(isValidInput(NULL));
}

TEST(ActionUsageTest, InvalidRInput) {
    EXPECT_FALSE(isValidInput("R, Student, String, 0"));
    EXPECT_FALSE(isValidInput("R, Student, Integer, 0"));
    EXPECT_FALSE(isValidInput("R, Student, Integer, -20"));
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

TEST(ActionUsageTest, ValidIInput) {
    EXPECT_TRUE(isValidIInput("I, Student, \"index1\", \"record1\""));
    EXPECT_TRUE(isValidIInput("I, Student, \"index1\", \"record1\"; \"index2\", \"record2\""));
}

TEST(ActionUsageTest, InvalidDInput) {
    EXPECT_FALSE(isValidInput("D, Student, a"));
    EXPECT_FALSE(isValidInput("D, Student,"));
}

TEST(ActionUsageTest, ValidDInput) {
    EXPECT_TRUE(isValidInput("D, Student, 10"));
    EXPECT_TRUE(isValidInput("D, Student, \"10\""));
}

TEST(ActionUsageTest, getRPayload) {
    auto p1 = getRPayload("R,RPayload, String, 200");
    EXPECT_STREQ(p1.name, "RPayload");
    EXPECT_EQ(p1.type, ValueType::String);
    EXPECT_EQ(p1.size, 200);
}

TEST(ActionUsageTest, getIPayload) {
    auto p1 = getIPayload("I, IPayload, 1, \"Record1\"; \"2\", \"Record2\"");
    EXPECT_STREQ(p1.name, "IPayload");
    ASSERT_EQ(p1.values.size(), 2);
    EXPECT_EQ(p1.values[0].first->type, ValueType::Integer);
    EXPECT_EQ(p1.values[0].first->IntValue, 1);
    EXPECT_STREQ(p1.values[0].second, "Record1");
    EXPECT_EQ(p1.values[1].first->type, ValueType::String);
    EXPECT_STREQ(p1.values[1].first->StrValue, "2");
    EXPECT_STREQ(p1.values[1].second, "Record2");
}
