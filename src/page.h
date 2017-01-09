#ifndef PAGE_HEADER
#define PAGE_HEADER

#include <cstdint>

/*
 * Page structure only affects how these functions are implemented.
 */
////// Page Structure ////////
typedef char Page;

struct Slot {
    int16_t offset;
    uint16_t length;
};

/*
 * Hopefully, changing this value won't break down other functions
 * However, PAGE_SIZE should be smaller than 2^15 (range of Slot->offset)
 * */
#define PAGE_SIZE 512

/*
 * Return number of records in the page.
 *
 * Notice that removing a record from the page won't decrease this value,
 * but reformatting the page (to keep it compact) will.
 * */
#define records_in_page(page) (*(uint16_t*)((char*)(page)+PAGE_SIZE-6))

/*
 * Return a pointer pointing to the beginning of the free space in the page
 * */
#define free_space_pointer(page) ((char*)(((((std::size_t)(page))>>32)<<32)|(*(uint32_t*)((char*)(page)+PAGE_SIZE-4))))

void set_free_space_pointer(Page*, char*);

/*
 * Given slot number, return the pointer to the slot
 * */
#define record_slot(page, sn) ((Slot*)((char*)(page)+PAGE_SIZE-(sn*4)-10))

#define prev_slot(slot) ((Slot*)(slot)-1)
#define next_slot(slot) ((Slot*)(slot)+1)

/*
 * Given slot number, return the offset of the record to the beginning to the page
 * */
#define record_offset(page, sn) (*(int16_t*)((char*)(page)+PAGE_SIZE-(sn)*4-10))

/*
 * Given slot number, return the length of the record
 * */
#define record_len(page, sn) (*(uint16_t*)((char*)(page)+PAGE_SIZE-(sn)*4-8))

/////// Page Structure //////

Page* create_page();
void delete_page(Page*);

/*
 * Add a record into the page.
 *
 * Return -1 if the page is full; slot number otherwise
 * */
int16_t add_record(Page*, const void*, unsigned int);
void remove_record(Page*, unsigned int);

/*
 * Mainly internel usage
 * Try to find a previous deleted slot that can store the new record
 * Note that if this function only check spaces that have been used
 * (and don't check free space)
 *
 * If this slot found, store the record and return the slot number
 * Return -1 otherwise
 * */
int16_t squeeze_record(Page*, const void*, unsigned int);

/*
 * Return the pointer to the beginning of the record.
 * Use record_len(page, sn) to get record length.
 * */
char* get_record(Page*, unsigned int);

#endif  // PAGE_HEADER
