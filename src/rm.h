#ifndef RECORD_MANAGER_HEADER
#define RECORD_MANAGER_HEADER

#include <vector>
#include "action.h"
#include "page.h"
#include <utility>

typedef std::pair<char*, unsigned int> RawData;

#define get_raw_pointer(x) (x.first)
#define get_raw_length(x) (x.second)

#define get_pid(rid) (rid >> 16)
#define get_sn(rid) (rid & 65535)
#define to_rid(pid, sn) ((((int32_t)pid) << 16) | (((int32_t)sn) & 65535))

RawData recordToData(const Record&, ValueType);
Record dataToRecord(RawData, ValueType);

class RecordManager {
public:
    RecordManager(ValueType);
    ~RecordManager();
    const unsigned int pageSize();
    const unsigned int getPageLength(unsigned int);
    const unsigned int size();

    int32_t addRecord(const Record&);
    void removeRecord(int32_t);
    Record getRecord(int32_t);

private:
    std::vector<Page*> pages;
    unsigned int _size;
    ValueType type;
};

#endif
