#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include <vector>

#include "library.h"

/*
 * # Delete a single record in the heap file given its record ID
 * $ delete <heapfile> <record_id> <page_size>

 * note: record_id has form <int:int> (page_id:slot)
 */

 int main(int argc, char* argv[])
 {
 	// input sanity check
 	if (argc != 4) {
 		fprintf(stderr, "Usage: %s <heapfile> <record_id> <page_size>\n", argv[0]);
        exit(1);
	}
	FILE *heapfile_fp;
	if ((heapfile_fp = fopen(argv[1], "r+")) == NULL) {
		fprintf(stderr, "cannot read file %s\n", argv[1]);
		exit(1);
	}

	char * record_id = argv[2];
	int data_page_id = atoi(strtok(record_id, ":"));
	int slot = atoi(strtok(NULL, ":"));

	int page_size = atoi(argv[3]);
	if (page_size <= 0) {
		fprintf(stderr, "page_size should be greater than %d.\n", NUM_ATTRS * ATTR_LEN);
		exit(1);
	}

	Heapfile *heapfile = (Heapfile *)malloc(sizeof(Heapfile));
	init_heapfile(heapfile, page_size, heapfile_fp);

	Slot *slot_ptr;

	// load the data page from heapfile into memory
	Page *data_page = (Page *)malloc(sizeof(Page));
	data_page->data = (void *)malloc(page_size);
	init_fixed_len_page(data_page, page_size, sizeof(Slot));
	read_page(heapfile, data_page_id, data_page);

	// delete the specific attribute with new_value
	slot_ptr = (Slot *)((char *)data_page->data + slot * sizeof(Slot));
	printf("slot flag = %c, record = %s \n", slot_ptr->flag, slot_ptr->record);

	slot_ptr->flag = '0';
	bzero(slot_ptr->record, sizeof(Slot));

	printf("slot flag = %c, record = %s \n", slot_ptr->flag, slot_ptr->record);
	
	// save the data page from memory to disk
	write_page(data_page, heapfile, data_page_id);

	// update directory page
	Page_entry *dir_page_entry, *data_page_entry;
	int dir_page_capacity = page_size / (OFFSET_LEN + FREESPACE_LEN);
	int data_page_capacity = page_size / sizeof(Slot);

	// load the directory page into memory and update it
	int dir_page_id = (data_page_id / dir_page_capacity) * dir_page_capacity;
	Page *dir_page = (Page *)malloc(sizeof(Page));
	dir_page->data = (void *)malloc(page_size);
	init_fixed_len_page(dir_page, page_size, OFFSET_LEN + FREESPACE_LEN);
	read_page(heapfile, dir_page_id, dir_page);
	dir_page_entry = (Page_entry *)dir_page->data;
	data_page_entry = dir_page_entry + data_page_id % dir_page_capacity;
	data_page_entry->freespace--;
	// save the dir page from memory to disk
	write_page(dir_page, heapfile, dir_page_id);

	free(data_page->data);
	free(data_page);
	free(dir_page->data);
	free(dir_page);	
	free(heapfile);
	fclose(heapfile_fp);

 	return 0;
 }



