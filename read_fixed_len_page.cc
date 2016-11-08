#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>

#include "library.h"

/**
 *load the page_file, and print out all records in the page in CSV format.

 *$ read_fixed_len_page <page_file> <page_size>
 **/

 int main(int argc, char* argv[])
 {
 	// input sanity check
 	if (argc != 3) {
		printf("usage: read_fixed_len_page <page_file> <page_size>\n");
		exit(-1);
	}
	FILE *page_fp;
	if ((page_fp = fopen(argv[1], "r")) == NULL) {
		printf("cannot read file %s\n", argv[1]);
		exit(-1);
	}	
	int page_size = atoi(argv[2]);
	if (page_size <= 0) {
		printf("page_size should be positive.\n");
		exit(-1);
	}

	Page *page;
	int slot_size = 1 + ATTRS_SIZE * ATTR_LEN;
	int page_capacity = page_size / slot_size;
	page_size = (page_size / slot_size) * slot_size;
	page = (Page *)malloc(sizeof(Page));
	page->data = (void *)malloc((page_size / slot_size) * slot_size);
	init_fixed_len_page(page, page_size, slot_size);
	
	int rec_count = 0;
	int page_count = 0;	
	struct timeb t_start, t_end;
	long start_in_ms, end_in_ms;
	int t_diff = 0;

	ftime(&t_start);
	start_in_ms = t_start.time * 1000 + t_start.millitm;

	int i, j;
	char *slot_ptr;
	char attr_value[ATTR_LEN + 1];
	bzero(attr_value, ATTR_LEN + 1);
	// read the page_file into page, one page each time
	while (fread(page->data, 1, page_size, page_fp) != 0) {
		page_count++;
		// loop each slot
		for (i=0; i<page_capacity; i++) {
			slot_ptr = (char *)page->data + i * slot_size;
			// if the slot contains record 
			if (strncmp(slot_ptr, "1", 1) == 0) {
				rec_count++;
				// print the record as csv format
				for (j=0; j<(ATTRS_SIZE - 1); j++) {
					strncpy(attr_value, slot_ptr + 1 + j * ATTR_LEN, ATTR_LEN);
					attr_value[ATTR_LEN + 1] = '\0';
					printf("%s,", attr_value);
					bzero(attr_value, ATTR_LEN + 1);
				}
				// print the last attribute value
				strncpy(attr_value, slot_ptr + 1 + j * ATTR_LEN, ATTR_LEN);
				attr_value[ATTR_LEN + 1] = '\0';
				printf("%s\n\n", attr_value);
				bzero(attr_value, ATTR_LEN + 1);
			}			
		}
	}

	ftime(&t_end);
	end_in_ms = t_end.time * 1000 + t_end.millitm;
	t_diff = end_in_ms - start_in_ms;
	
	free(page->data);
	free(page);
	fclose(page_fp);

	printf("NUMBER OF RECORDS: %d\n", rec_count);
	printf("NUMBER OF PAGES: %d\n", page_count);
	printf("TIME: %d\n", t_diff);

 	return 0;
 }

