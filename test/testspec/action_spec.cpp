#include <cstring>
#include <vector>
#include "gtest/gtest.h"
#include "action.h"

using namespace std;

TEST(ActionSpecTest, ValueSpec) {
    // Test default values
    Value value;
    EXPECT_EQ(value.type, ValueType::Undefined);
    EXPECT_EQ(value.IntValue, 0);
    EXPECT_TRUE(value.n == NULL);

    Value ivalue(9, &value);
    EXPECT_EQ(ivalue.type, ValueType::Integer);
    EXPECT_EQ(ivalue.IntValue, 9);
    EXPECT_EQ(ivalue.n, &value);

    char* str = new char[12];
    strcpy(str, "StringValue");

    Value svalue(str, &value);
    EXPECT_EQ(svalue.type, ValueType::String);
    EXPECT_STREQ(svalue.StrValue, "StringValue");
    EXPECT_EQ(svalue.n, &value);
    delete[] str;
}

TEST(ActionSpecTest, PayloadSpecs) {
    RPayload rpayload = {
        .name = "RPayload"
    };

    // Test default values
    EXPECT_STREQ(rpayload.name, "RPayload");
    EXPECT_EQ(rpayload.type, ValueType::Undefined);
    EXPECT_EQ(rpayload.size, 0);


    IPayload ipayload = {
        .name = "IPayload"
    };

    // Test default values
    EXPECT_STREQ(ipayload.name, "IPayload");
    EXPECT_TRUE(ipayload.values == NULL);

    DPayload dpayload = {
        .name = "DPayload"
    };

    // Test default values
    EXPECT_STREQ(dpayload.name, "DPayload");
    EXPECT_EQ(dpayload.value.type, ValueType::Undefined);
    EXPECT_EQ(dpayload.value.IntValue, 0);
    EXPECT_TRUE(dpayload.value.n == NULL);
}

TEST(ActionSpecTest, ActionSpec) {
    RPayload rpayload = {
        .name = "RPayload"
    };

    IPayload ipayload = {
        .name = "IPayload"
    };

    DPayload dpayload = {
        .name = "DPayload"
    };

    Action actionr(rpayload);
    EXPECT_STREQ(actionr.payload.r.name, "RPayload");
    EXPECT_EQ(actionr.type, ActionType::R);

    Action actioni(ipayload);
    EXPECT_STREQ(actioni.payload.i.name, "IPayload");
    EXPECT_EQ(actioni.type, ActionType::I);

    Action actiond(dpayload);
    EXPECT_STREQ(actiond.payload.d.name, "DPayload");
    EXPECT_EQ(actiond.type, ActionType::D);
}
