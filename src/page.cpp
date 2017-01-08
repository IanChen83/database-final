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

int16_t
add_record(Page* page, const void* record, unsigned int len) {
    auto p = free_space_pointer(page);
    auto records_num = records_in_page(page);

    if(p + len + 6 + 4 * records_num > page + PAGE_SIZE) {
        return squeeze_record(page, record, len);
    }

    memcpy(p, record, len);
    Slot* s = record_slot(page, records_num);
    s->offset = p - page;
    s->length = (uint32_t)len;
    ++records_in_page(page);
    set_free_space_pointer(page, free_space_pointer(page) + len);
    return records_num;
}

void
remove_record(Page* page, unsigned int num) {
    record_offset(page, num) = -record_offset(page, num) - 1;
}

int16_t
squeeze_record(Page* page, const void* record, unsigned int len) {
    auto total_records = records_in_page(page);
    auto it = total_records - 1;

    while(it >= 0) {
        auto slot = record_slot(page, it);
        if(slot->offset >= 0 || slot->length < len) {
            --it;
            continue;
        }

        // offset is currently negative
        char* p = page - record_offset(page, it) - 1;

        memcpy(p, record, len);
        slot->offset = p - page;
        slot->length = (uint32_t)len;

        return it;
    }

    return -1;
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


