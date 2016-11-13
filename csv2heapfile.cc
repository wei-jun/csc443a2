#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>

#include "library.h"

/**
 *Build heap file from CSV file
 *csv2heapfile <csv_file> <heapfile> <page_size>
 */


int main(int argc, char* argv[])
 {
    // input sanity check
 	if (argc != 4) {
		fprintf(stderr, "Usage: %s <csv_file> <heapfile> <page_size>\n", argv[0]);
		exit(-1);
	}
	FILE *csv_fp;
	if ((csv_fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "cannot open file %s\n", argv[1]);
		exit(-1);
	}	
	int page_size = atoi(argv[3]);
	if (page_size <= NUM_ATTRS * ATTR_LEN) {
		fprintf(stderr, "page_size should be greater than %d.\n", NUM_ATTRS * ATTR_LEN);
		exit(-1);
	}
	// page_size = (page_size / sizeof(Slot)) * sizeof(Slot);

	FILE *heapfile_fp = fopen(argv[2], "w");
	Heapfile *heapfile = (Heapfile *)malloc(sizeof(Heapfile));
	heapfile->file_ptr = heapfile_fp;
	heapfile->page_size = page_size;

	char line[MAXLINE];  // line in csv file
	char row[MAXLINE];   // row in page
	bzero(line, MAXLINE);
	bzero(row, MAXLINE);
	Record *record;
	Slot *slot_ptr;

	PageID dir_page_id = 0;
	PageID data_page_id = 1;
	Page_entry *dir_page_entry, *data_page_entry;
	int dir_page_capacity = page_size / (OFFSET_LEN + FREESPACE_LEN);
	int data_page_capacity = page_size / sizeof(Slot);

	Page *dir_page = (Page *)malloc(sizeof(Page));
	dir_page->data = (void *)malloc(page_size);
	init_fixed_len_page(dir_page, page_size, OFFSET_LEN + FREESPACE_LEN);
	dir_page_entry = (Page_entry *)dir_page->data;
	dir_page_entry->offset = 0;
	dir_page_entry->freespace = dir_page_capacity - 1;

	data_page_entry = dir_page_entry + 1;
	data_page_entry->offset = 0;
	data_page_entry->freespace = data_page_capacity;
	
	Page *data_page = (Page *)malloc(sizeof(Page));
	data_page->data = (void *)malloc(page_size);
	init_fixed_len_page(data_page, page_size, sizeof(Slot));

	int rec_count = 0;
	int page_count = 0;	
	struct timeb t_start, t_end;
	long start_in_ms, end_in_ms;
	int t_diff = 0;

	ftime(&t_start);
	start_in_ms = t_start.time * 1000 + t_start.millitm;
	// read csv file line by line
	int j = 0;
	while(fgets(line, MAXLINE, csv_fp) != NULL) {
		char *curr_attr = strtok(line, ",");
		int i = 0;
		// remove comma "," from the line and save into row
		while (curr_attr) {
			strncpy(row + i * ATTR_LEN, curr_attr, ATTR_LEN);
			curr_attr = strtok(NULL, ",");
			i++;			
		}

		/*
		// use the row to construct a record
		fixed_len_read(row, NUM_ATTRS * ATTR_LEN, record);

		// write the record into a page
		write_fixed_len_page(page, j, record);
		*/
		
		// write the row into a page
		slot_ptr = (Slot *)((char *)data_page->data + j * sizeof(Slot));
		slot_ptr->flag = '1';
		memcpy(slot_ptr->record, row, sizeof(Slot) - 1);
		data_page_entry->freespace--;
		rec_count++;
		j++;

		bzero(line, MAXLINE);
	    bzero(row, MAXLINE);
		
		// if the data page is full, write to heapfile		
		if (j == data_page_capacity) {
			write_page(data_page, heapfile, data_page_id);

			bzero(data_page->data, page_size);
			data_page_entry->offset = data_page_id * page_size;
			page_count++;
			j = 0;
			data_page_id++;
			if (dir_page_entry->freespace > 0) {
				dir_page_entry->freespace--;
				data_page_entry++;
				data_page_entry->offset = 0;
				data_page_entry->freespace = data_page_capacity;
			}
			// dir page is full, write to heapfile
			else {
				dir_page_entry->offset = (dir_page_id + dir_page_capacity) * page_size;
				write_page(dir_page, heapfile, dir_page_id);
				page_count++;
				dir_page_id += dir_page_capacity;
				bzero(dir_page->data, page_size);
				dir_page_entry = (Page_entry *)dir_page->data;
	            dir_page_entry->offset = 0;
	            dir_page_entry->freespace = dir_page_capacity - 1;

	            data_page_id = dir_page_id + 1;
	            data_page_entry = dir_page_entry + 1;
	            data_page_entry->offset = 0;
	            data_page_entry->freespace = data_page_capacity;
			}
		}
	}
	// write the last not-full page if applicable
	if (j != 0) {
		write_page(data_page, heapfile, data_page_id);
		page_count++;
	}
	write_page(dir_page, heapfile, dir_page_id);
	page_count++;	

	ftime(&t_end);
	end_in_ms = t_end.time * 1000 + t_end.millitm;
	t_diff = end_in_ms - start_in_ms; 

	free(dir_page->data);
	free(dir_page);
	free(data_page->data);
	free(data_page);
	fclose(csv_fp);
	fclose(heapfile_fp);

	printf("NUMBER OF RECORDS: %d\n", rec_count);
	printf("NUMBER OF PAGES: %d\n", page_count);
	printf("TIME: %d ms\n", t_diff);

 	return 0;
 }




