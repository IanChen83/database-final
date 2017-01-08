#include "gtest/gtest.h"
#include "page.h"
#include <iostream>

using namespace std;

TEST(PageTest, InitPageSpec) {
    Page* page = create_page();
    ASSERT_TRUE(page != NULL);
    EXPECT_EQ(records_in_page(page), 0);
    EXPECT_EQ(free_space_pointer(page) - page, 0);
    delete_page(page);
}

TEST(PageTest, AddRecord) {
    Page* page = create_page();
    const char* a = "asdf";
    auto r = add_record(page, a, 5);

    ASSERT_EQ(r, 0);
    EXPECT_EQ(records_in_page(page), 1);
    EXPECT_EQ(record_offset(page, r), 0);
    EXPECT_EQ(record_len(page, r), 5);

    auto r2 = add_record(page, a, 5);
    ASSERT_EQ(r2, 1);
    EXPECT_EQ(records_in_page(page), 2);
    EXPECT_EQ(record_offset(page, r2), 5);
    EXPECT_EQ(record_len(page, r2), 5);

    delete_page(page);
}

TEST(PageTest, RemoveRecord) {
    Page* page = create_page();
    const char* a = "asdf";
    auto r = add_record(page, a, 5);
    auto r2 = add_record(page, a, 5);
    remove_record(page, 0);
    EXPECT_EQ(record_offset(page, 0), -1);
    EXPECT_EQ(record_len(page, 0), 5);
    remove_record(page, 1);
    EXPECT_EQ(record_offset(page, 1), -6);
    EXPECT_EQ(record_len(page, 1), 5);
    delete_page(page);
}

TEST(PageTest, GetRecord) {
    Page* page = create_page();
    const char* a = "asdf";
    const char* b = "qwer";
    auto r = add_record(page, a, 5);
    auto r2 = add_record(page, b, 5);

    auto x = get_record(page, 0);
    auto x2 = get_record(page, 1);

    ASSERT_TRUE(x != NULL);
    ASSERT_TRUE(x2 != NULL);

    EXPECT_TRUE(x[0] == 'a');
    EXPECT_TRUE(x2[0] == 'q');

    delete_page(page);
}


TEST(PageTest, SqueezeRecord) {
    Page* page = create_page();
    const char* a = "asdf";
    auto r = add_record(page, a, 5);
    auto r2 = add_record(page, a, 5);
    remove_record(page, 0);

    auto r3 = squeeze_record(page, a, 5);
    EXPECT_EQ(r3, 0);
    EXPECT_EQ(record_offset(page, r3), 0);
    EXPECT_EQ(record_len(page, r3), 5);

    auto x = get_record(page, r3);
    EXPECT_STREQ(x, a);

    delete_page(page);
}
