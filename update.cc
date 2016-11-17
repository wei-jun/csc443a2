#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include <vector>

#include "library.h"

/*
 * Update one attribute of a single record in the heap file given its record ID
 * <attribute_id> is the index of the attribute to be updated (e.g. 0 for the first attribute, 
 * 1 for the second attribute, etc.) <new_value> will have the same fixed length (10 bytes)
 * record_id has format <int:int>
 * $ update <heapfile> <record_id> <attribute_id> <new_value> <page_size>
 */

 int main(int argc, char* argv[])
 {
 	// input sanity check
 	if (argc != 6) {
 		fprintf(stderr, "Usage: %s <heapfile> <record_id> <attribute_id> <new_value> <page_size>\n", argv[0]);
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

	int attribute_id = atoi(argv[3]);
    char * new_value = argv[4];

	int page_size = atoi(argv[5]);
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

	// update the specific attribute with new_value
	slot_ptr = (Slot *)((char *)data_page->data + slot * sizeof(Slot));
	strncpy(slot_ptr->record + attribute_id * ATTR_LEN, new_value, ATTR_LEN);
	
	// save the data page from memory to disk
	write_page(data_page, heapfile, data_page_id);

	free(data_page->data);
	free(data_page);	
	free(heapfile);
	fclose(heapfile_fp);

 	return 0;
 }



