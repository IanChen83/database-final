#include <string>
#include <tuple>
#include <utility>
#include "gtest/gtest.h"
#include "rm.h"
#include "value.h"

using namespace std;

TEST(RecordManager, rid) {
    EXPECT_EQ(get_pid(to_rid(2, 5)), 2);
    EXPECT_EQ(get_sn(to_rid(2, 5)), 5);
}

TEST(RecordManager, recordToData) {
    Record record  = make_tuple(createValue(12), "asdf");

    auto x = recordToData(record, ValueType::Integer);
    EXPECT_EQ(x.second, 10);
    EXPECT_EQ(*(int*)x.first, 12);
    EXPECT_STREQ(x.first + 5, "asdf");

    removeValue(std::get<0>(record));
}

TEST(RecordManager, dataToRecord) {
    Record record  = make_tuple(createValue(12), "asdf");

    auto x = dataToRecord(recordToData(record, ValueType::Integer), ValueType::Integer);
    EXPECT_EQ(std::get<0>(x)->type, ValueType::Integer);
    EXPECT_EQ(std::get<0>(x)->IntValue, 12);
    EXPECT_EQ(std::get<1>(x), "asdf");

    removeValue(std::get<0>(record));
}

