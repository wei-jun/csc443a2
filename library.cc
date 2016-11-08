#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "library.h"

/*
#include <vector>
typedef const char* V;
typedef std::vector<V> Record;

#define NUM_ATTRS 100 // number of attributes
#define ATTR_LEN  10 //  lenght of each attribute
#define MAXLINE   1200

typedef struct {
    void *data;
    int page_size;
    int slot_size;
} Page;

typedef struct {
    FILE *file_ptr;
    int page_size;
} Heapfile

typedef int PageID;
 
typedef struct {
    int page_id;
    int slot;
} RecordID;
*/


/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record) {
	//int i = sizeof(record);
	//return i;
	return 0;
}

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf) {
	//std::vector<V> Record = {'aaaaa aaaaa', 'bbbbb bbbbb', ... 'zzzzz zzzzz'};
	//*buf = 'aaaaaaaaaabbbbbbbbbb ... zzzzzzzzzz'
	/*
	char *buf = (char*)buf;
	std::vector<V>::iterator it;
	for(it = Record.begin(); it != Record.end(); ++it) {
 	    *buf = *it;
	    *buf += 10;
	}
	*/
	return;	
}

/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 * Input: fixed_len_read('aaaaabbbbbccccc', 1000, {});
 * Output: {'aaaaa', 'bbbbb', 'ccccc'}
 */
void fixed_len_read(void *buf, int size, Record *record) {
	/*
	for (int i=0; i < size; i+10){
		char *temp = buf[i, i+9];
		Record.push_back(temp);
	}
	*/
}


/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size) 
{	
	page->page_size = page_size;
	page->slot_size = slot_size;
	bzero(page->data, page_size);
}
 
/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page)
{
	return page->page_size / page->slot_size;
}
 
/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page) 
{
	int i;
	int count = 0;
	char *slot_ptr;
	char flag;  // 1=occupied, 0=free space
	for (i=0; i<(page->page_size)/(page->slot_size); i++) {
		slot_ptr = (char *)page->data + i * page->slot_size;
		strncpy(&flag, slot_ptr, 1);
		if ( strcmp(&flag, "0") == 0) {
			count++;
		}
	}
	return count;
}
 
/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r)
{
	if (fixed_len_page_freeslots(page) == 0) {
		return -1;
	}
	int i;
	char *slot_ptr;
	for (i=0; i<(page->page_size)/(page->slot_size); i++) {
		slot_ptr = (char *)page->data + i * page->slot_size;
		if ( strncmp(slot_ptr, "0", 1) == 0) {
			strncpy(slot_ptr, "1", 1);
			fixed_len_write(r, slot_ptr + 1);
			return i;
		}
	}
	return -1;
}
 
/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r)
{
	char *slot_ptr = (char *)page->data + slot * page->slot_size;
	strncpy(slot_ptr, "1", 1);
	fixed_len_write(r, slot_ptr + 1);
	return;
}
 
/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r)
{
	char *slot_ptr = (char *)page->data + slot * page->slot_size;
	fixed_len_read(slot_ptr + 1, page->slot_size - 1, r);	
	return;
}

/**
 * Initalize a heapfile to use the file and page size given.
 */
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file)
{
	return;
}

/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile)
{
	int page_id;
	return page_id;
}

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page)
{
	return;
}

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid)
{
	return;
}

/*
class RecordIterator {
    public:
    RecordIterator(Heapfile *heapfile);
    Record next();
    bool hasNext();
};
*/







