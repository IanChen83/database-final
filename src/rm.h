#ifndef RECORD_MANAGER_HEADER
#define RECORD_MANAGER_HEADER

#include <vector>
#include "action.h"
#include "page.h"

#define get_pid(rid) (rid >> 16)
#define get_sn(rid) (rid & 65535)
#define to_rid(pid, sn) ((((int32_t)pid) << 16) | (((int32_t)sn) & 65535))


class RecordManager {
public:
    RecordManager();
    ~RecordManager();
    const unsigned int pageSize();
    const unsigned int size();

    int32_t addRecord(const Record&);
    void removeRecord(int32_t);
    Record getRecord(int32_t);

private:
    std::vector<Page*> pages;
    unsigned int _size;
};

#endif
