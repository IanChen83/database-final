#include <cstring>
#include <iostream>
#include "page.h"

using namespace std;

void
set_free_space_pointer(Page* page, char* p) {
    *(uint32_t*)(page + PAGE_SIZE - 4) = (uint32_t)(((size_t)p << 32) >> 32);
}

Page*
create_page() {
    Page* page = new Page[PAGE_SIZE]();
    set_free_space_pointer(page, page);
    return page;
}

void
delete_page(Page* page) {
    delete[] page;
}

unsigned int
add_record(Page* page, const char* record, unsigned int len) {
    auto p = free_space_pointer(page);
    auto records_num = records_in_page(page);

    if(p + len + 6 + 4 * records_num > page + PAGE_SIZE) {
        return 0;
    }

    memcpy(p, record, len);
    record_offset(page, records_num) = p - page;
    record_len(page, records_num) = (uint32_t)len;
    unsigned int ret = ++records_in_page(page);
    set_free_space_pointer(page, free_space_pointer(page) + len);
    return ret;
}

void
remove_record(Page* page, unsigned int num) {
    record_offset(page, num) = -1;
    record_len(page, num) = 0;
}

char*
get_record(Page* page, unsigned int num) {
    if(num >= records_in_page(page))
        return NULL;
    auto offset = record_offset(page, num);
    if(offset != -1)
        return page + offset;
    else
        return NULL;
}


