#include <tuple>
#include "page.h"
#include "action.h"
#include "value.h"
#include "rm.h"
#include <cstdlib>
#include <cstring>

using namespace std;

RawData
recordToData(const Record& record, ValueType type) {
    Value* v;
    string s;
    tie(v, s) = record;

    unsigned int len = 0, keylen = 0;
    if(type == ValueType::Integer) {
        keylen = sizeof(int);
    }else if(type == ValueType::String) {
        keylen = strlen(v->StrValue);
    }

    // key length + split char + data length
    len = keylen + 1 + s.size() + 1;

    // Prepare data
    char* data = new char[len]();
    if(type == ValueType::Integer) {
        *(int*)data = v->IntValue;
    }else if(type == ValueType::String) {
        strcpy(data, v->StrValue);
    }

    data[keylen] = '\0';
    memcpy(data + keylen + 1, s.c_str(), s.size());

    return make_pair(data, len);
}

Record
dataToRecord(RawData data, ValueType type) {
    unsigned int len = 0, keylen = 0;
    Value* value;
    if(type == ValueType::Integer) {
        keylen = sizeof(int);
        value = createValue(*(int*)get_raw_pointer(data));
    }else if(type == ValueType::String) {
        keylen = strlen(get_raw_pointer(data));
        value = createValue(get_raw_pointer(data));
    }

    len = get_raw_length(data) - 1 - keylen - 1;
    return Record(value, string(get_raw_pointer(data) + keylen + 1, len));
}

RecordManager::RecordManager(ValueType _type) : _size(0), type(_type) {}

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

const unsigned int
RecordManager::getPageLength(unsigned int pid) {
    return records_in_page(pages[pid]);
}

int32_t
RecordManager::addRecord(const Record& record) {
    auto data_len = recordToData(record, type);
    if(data_len.second >= PAGE_SIZE) {
        return -1;
    }

    int16_t r = -1, pid = 0;
    for(auto page: pages) {
        r = add_record(page, data_len.first, data_len.second);
        if(r != -1) break;
        ++pid;
    }

    if(r == -1) {
        // No available space, create new page
        auto x = create_page();
        pid = pages.size();
        r = add_record(x, data_len.first, data_len.second);
        pages.push_back(x);
    }

    ++_size;
    delete[] data_len.first;
    return to_rid(pid, r);
}

Record
RecordManager::getRecord(int32_t rid) {
    int16_t pid = get_pid(rid), sn = get_sn(rid);
    auto page = pages[pid];

    char* data = get_record(page, sn);
    unsigned int len = record_len(page, sn);

    return dataToRecord(std::make_pair(data, len), type);
    // return;
}

void
RecordManager::removeRecord(int32_t rid) {
    int16_t pid = get_pid(rid);
    int16_t sn = get_sn(rid);

    auto x = pages[pid];
    remove_record(x, sn);
    --_size;
}
