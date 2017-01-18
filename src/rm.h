#ifndef RECORD_MANAGER_HEADER
#define RECORD_MANAGER_HEADER

#include <vector>
#include "page.h"

#define get_pid(rid) (rid >> 16)
#define get_sn(rid) (rid & 65535)
#define to_rid(pid, sn) ((((int32_t)pid) << 16) | (((int32_t)sn) & 65535))

struct RecordResponse
{
    void* data;
    unsigned int length;
};

class RecordManager {
public:
    RecordManager();
    ~RecordManager();
    const unsigned int pageSize();
    const unsigned int size();

    int32_t addRecord(const void*, unsigned int);
    void removeRecord(int32_t);
    RecordResponse getRecord(int32_t);

private:
    std::vector<Page*> pages;
    unsigned int _size;
};

#endif
