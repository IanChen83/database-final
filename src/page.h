#ifndef PAGE_HEADER
#define PAGE_HEADER

#include <cstdint>

#define PAGE_SIZE 512

typedef char Page;

#define records_in_page(page) (*(uint16_t*)((char*)(page)+PAGE_SIZE-6))

#define free_space_pointer(page) ((char*)(((((std::size_t)(page))>>32)<<32)|(*(uint32_t*)((char*)(page)+PAGE_SIZE-4))))
void set_free_space_pointer(Page*, char*);

#define record_offset(page, sn) (*(int16_t*)((char*)(page)+PAGE_SIZE-(sn)*4-10))
#define record_len(page, sn) (*(uint16_t*)((char*)(page)+PAGE_SIZE-(sn)*4-8))

Page* create_page();
void delete_page(Page*);

unsigned int add_record(Page*, const char*, unsigned int);
void remove_record(Page*, unsigned int);
char* get_record(Page*, unsigned int);

#endif  // PAGE_HEADER
