#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>

typedef char* V;
typedef std::vector<V> Record;

#define NUM_ATTRS 100 // number of attributes
#define ATTR_LEN  10 //  lenght of each attribute
#define MAXLINE   1200
#define OFFSET_LEN     8 // bytes of page_offset
#define FREESPACE_LEN  4 // bytes of freespace

typedef struct {
    void *data;
    int page_size;
    int slot_size;
} Page;

typedef struct {
	char flag;  // '0' - free; '1' - occupied
	char record[NUM_ATTRS * ATTR_LEN];
} Slot;

typedef struct {
    FILE *file_ptr;
    int page_size;
} Heapfile;

typedef int PageID;
 
typedef struct {
    int page_id;
    int slot;
} RecordID;

typedef struct {
	unsigned long offset;  // page offset from the beginning of heapfile in bytes
	int freespace;  // number of free slots
} Page_entry;

/**
 * Idea: Heapfile file_ptr points to the first page of the heapfile, the first page 
 * is always a directory page; directory page has the same page size as regular 
 * data(records) page, but smaller slot size(12 bytes) and bigger page capacity(number 
 * of slots); each slot stores one page_entry(page_offset, freespace); page_offset is
 * 8 bytes, freespace(number of slots) is 4 bytes; 
 * The first slot of each directory page stores offset for the next directory page, 
 * (0 indicates no next directory page), and the freespace for this directory page.
 * From second slot and on, each slot stores a data page's offset and freespace.
 */

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record);

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf);

/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record);

/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size);
 
/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page);
 
/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page);
 
/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r);
 
/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r);
 
/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r);

/**
 * Initalize a heapfile to use the file and page size given.
 */
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file);

/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile);

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page);

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid);

/**
 * enumerate records of a heap file
 */

class RecordIterator {
	Heapfile *heapfile;
	Page *cur_dir_page;
	Page *cur_data_page;
	RecordID cur_record_id;
    public:
    RecordIterator(Heapfile *heapfile);
    Record next();
    bool hasNext();
};



