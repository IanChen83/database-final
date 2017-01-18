#include "page.h"
#include "rm.h"

RecordManager::RecordManager() : _size(0) {}

RecordManager::~RecordManager() {
    for(auto& x: pages) {
        delete_page(x);
    }
}

const unsigned int
RecordManager::pageSize() {
    return pages.size();
}

const unsigned int
RecordManager::size() {
    return _size;
}

int32_t
RecordManager::addRecord(const void* data, unsigned int len) {
    if(len > PAGE_SIZE) return -1;
    int16_t r = -1, pid = pages.size() - 1;

    for(auto page: pages) {
        r = add_record(page, data, len);
        if(r == -1) continue;
    }

    if(r == -1) {
        // No available space, create new page
        auto x = create_page();
        pid = pages.size() - 1;
        r = add_record(x, data, len);
        pages.push_back(x);
    }

    ++_size;
    return to_rid(pid, r);
}

RecordResponse
RecordManager::getRecord(int32_t rid) {
    int16_t pid = get_pid(rid), sn = get_sn(rid);
    auto page = pages[pid];
    RecordResponse ret = {
        .data = get_record(page, sn),
        .length = record_len(page, sn)
    };
    return ret;
}

void
RecordManager::removeRecord(int32_t rid) {
    int16_t pid = get_pid(rid);
    int16_t sn = get_sn(rid);

    auto x = pages[pid];
    remove_record(x, sn);
    --_size;
}
