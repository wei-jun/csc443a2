#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "library.h"


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
	
	Slot *slot_ptr;
	for (i=0; i<(page->page_size/page->slot_size); i++) {
		slot_ptr = (Slot *)((char *)page->data + i * page->slot_size);
		if (slot_ptr->flag == '0') {
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
	Slot *slot_ptr;
	for (i=0; i<(page->page_size)/(page->slot_size); i++) {
		slot_ptr = (Slot *)((char *)page->data + i * page->slot_size);
		if ( slot_ptr->flag == '0') {
			slot_ptr->flag = '1';
			fixed_len_write(r, slot_ptr->record);
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
	if (slot < 0 || slot >= page->page_size / page->slot_size) {
		fprintf(stderr, "slot number out of range!\n");
		exit(-1);
	}
	
	Slot *slot_ptr = (Slot *)((char *)page->data + slot * page->slot_size);
	slot_ptr->flag = '1';
	fixed_len_write(r, slot_ptr->record);

	return;
}
 
/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r)
{
	if (slot < 0 || slot >= page->page_size / page->slot_size) {
		fprintf(stderr, "slot number out of range!\n");
		exit(1);
	}
	char *slot_ptr = (char *)page->data + slot * page->slot_size;
	fixed_len_read(slot_ptr + 1, page->slot_size - 1, r);

	return;
}

/**
 * Initalize a heapfile to use the file and page size given.
 */
void init_heapfile(Heapfile *heapfile, int page_size, FILE *file)
{
	heapfile->file_ptr = file;
	heapfile->page_size = page_size;
	return;
}

/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile)
{
	PageID dir_page_id = 0;
	PageID data_page_id = 0;
	Page_entry *dir_page_entry, *data_page_entry;
	int dir_page_capacity = heapfile->page_size / (OFFSET_LEN + FREESPACE_LEN);
	
    // allocate a new page to be added in the heapfile
	Page *new_page = (Page *)malloc(sizeof(Page));
	new_page->data = (void *)malloc(heapfile->page_size);
	init_fixed_len_page(new_page, heapfile->page_size, sizeof(Slot));

	// allocate a dir page for reading a dir page from heapfile
	Page *dir_page = (Page *)malloc(sizeof(Page));
	dir_page->data = (void *)malloc(heapfile->page_size);
	init_fixed_len_page(dir_page, heapfile->page_size, OFFSET_LEN + FREESPACE_LEN);

	// read the first page of the heap file, it is a dir page
	read_page(heapfile, dir_page_id, dir_page);

	// find a directory page for this new page entry
	dir_page_entry = (Page_entry *)dir_page->data;
	while (dir_page_entry->freespace < 1 && dir_page_entry->offset != 0) {
		dir_page_id += dir_page_entry->offset / heapfile->page_size;
		bzero(dir_page->data, heapfile->page_size);
		read_page(heapfile, dir_page_id, dir_page);
		dir_page_entry = (Page_entry *)dir_page->data;
	}
	//if find a dir page with freespace
	if (dir_page_entry->freespace >= 1) {
		dir_page_entry->freespace--;
		data_page_entry = dir_page_entry + 1;
		data_page_id = dir_page_id + 1;

		while (data_page_entry->offset != 0) {
			data_page_entry++;
			data_page_id++;
		}
		// update the free slot
		data_page_entry->offset = data_page_id * heapfile->page_size;
		data_page_entry->freespace = heapfile->page_size / sizeof(Slot);
		// write the modified dir page back to heap file
		write_page(dir_page, heapfile, dir_page_id);
		// write the new page in heapfile
		write_page(new_page, heapfile, data_page_id);
	}
	// if no dir page with freespace, create a new dir page, and add to the heap file
	//if (dir_page_entry->offset == 0) {
	else {
		// update the last full dir page offset
		dir_page_entry->offset = (dir_page_id + dir_page_capacity) * heapfile->page_size;
		// create new dir page
		dir_page_id += dir_page_capacity;
		bzero(dir_page->data, heapfile->page_size);
		dir_page_entry = (Page_entry *)dir_page->data;
		dir_page_entry->offset = 0;
		dir_page_entry->freespace = dir_page_capacity - 2;

		data_page_entry = dir_page_entry + 1;
		data_page_id = dir_page_id + 1;
		data_page_entry->offset = data_page_id * heapfile->page_size;
		data_page_entry->freespace = heapfile->page_size / sizeof(Slot);
		// write the new dir page to heap file
		write_page(dir_page, heapfile, dir_page_id);
		//write the new page to heap file
		write_page(new_page, heapfile, data_page_id);
	}
	free(dir_page->data);
	free(dir_page);
	free(new_page->data);
	free(new_page);

	return data_page_id;
}

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page)
{
	fseek(heapfile->file_ptr, pid * heapfile->page_size, SEEK_SET);
	fread(page->data, 1, heapfile->page_size, heapfile->file_ptr);
	fseek(heapfile->file_ptr, 0, SEEK_SET);
	
}

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid)
{
	fseek(heapfile->file_ptr, pid * heapfile->page_size, SEEK_SET);
	fwrite(page->data, 1, heapfile->page_size, heapfile->file_ptr);
	fseek(heapfile->file_ptr, 0, SEEK_SET);

}

/*
class RecordIterator {
    public:
    RecordIterator(Heapfile *heapfile);
    Record next();
    bool hasNext();
};
*/







