#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include <vector>

#include "library.h"

/**
 *# Insert all records in the CSV file to a heap file
 *$ insert <heapfile> <csv_file> <page_size>
 */

int main(int argc, char* argv[])
 {
    // input sanity check
 	if (argc != 4) {
		fprintf(stderr, "Usage: %s <heapfile> <csv_file> <page_size>\n", argv[0]);
		exit(-1);
	}
	FILE *heapfile_fp;
	if ((heapfile_fp = fopen(argv[1], "r+")) == NULL) {
		fprintf(stderr, "cannot open file %s\n", argv[1]);
		exit(-1);
	}	
	FILE *csv_fp;
	if ((csv_fp = fopen(argv[2], "r")) == NULL) {
		fprintf(stderr, "cannot open file %s\n", argv[2]);
		exit(-1);
	}	
	int page_size = atoi(argv[3]);
	if (page_size <= NUM_ATTRS * ATTR_LEN) {
		fprintf(stderr, "page_size should be greater than %d.\n", NUM_ATTRS * ATTR_LEN);
		exit(-1);
	}

	Heapfile *heapfile = (Heapfile *)malloc(sizeof(Heapfile));
	init_heapfile(heapfile, page_size, heapfile_fp);

	char line[MAXLINE];  // line in csv file
	char row[MAXLINE];   // row in page
	bzero(line, MAXLINE);
	bzero(row, MAXLINE);
	Record record;

	PageID dir_page_id = 0;
	PageID data_page_id = 1;
	Page_entry *dir_page_entry, *data_page_entry;
	int dir_page_capacity = page_size / (OFFSET_LEN + FREESPACE_LEN);
	int data_page_capacity = page_size / sizeof(Slot);

    // load the first directory page into memory
	Page *dir_page = (Page *)malloc(sizeof(Page));
	dir_page->data = (void *)malloc(page_size);
	init_fixed_len_page(dir_page, page_size, OFFSET_LEN + FREESPACE_LEN);
	read_page(heapfile, dir_page_id, dir_page);
	dir_page_entry = (Page_entry *)dir_page->data;
	data_page_entry = dir_page_entry + 1;
	
	// load first data page from heapfile into memory
	Page *data_page = (Page *)malloc(sizeof(Page));
	data_page->data = (void *)malloc(page_size);
	init_fixed_len_page(data_page, page_size, sizeof(Slot));
	data_page_id = data_page_entry->offset / page_size;
	read_page(heapfile, data_page_id, data_page);

	int rec_count = 0;
	struct timeb t_start, t_end;
	long start_in_ms, end_in_ms;
	int t_diff = 0;

	ftime(&t_start);
	start_in_ms = t_start.time * 1000 + t_start.millitm;

    // read csv file line by line
	while(fgets(line, MAXLINE, csv_fp) != NULL) {
		char *curr_attr = strtok(line, ",");
		int i = 0;
		// remove comma "," from the line and save into row
		while (curr_attr) {
			strncpy(row + i * ATTR_LEN, curr_attr, ATTR_LEN);
			curr_attr = strtok(NULL, ",");
			i++;			
		}
		// save the row to a record
		fixed_len_read((void *)row, sizeof(Slot) -1, &record);

		rec_count++;
		bzero(line, MAXLINE);
	    bzero(row, MAXLINE);

		// try to insert the record into the current data page
		if (add_fixed_len_page(data_page, &record) != -1) {
			// printf("record added in current data page\n");
			data_page_entry->freespace--;
		}
		else {
			// save current data page into disk
			// printf("write current data page into disk\n");
			write_page(data_page, heapfile, data_page_id);
			// find a page with free slot in current dir page to insert the record
			int found = 0;
			int i;
			for (i = 1; i < dir_page_capacity; i++) {
				data_page_entry = dir_page_entry + i;

				if ((data_page_entry->freespace > 0) && 
					(data_page_entry->freespace <= data_page_capacity) &&
					(data_page_entry->offset > 0)) {

					data_page_id = data_page_entry->offset / heapfile->page_size;
					found = 1;
					// printf("page with freespace found in current dir page\n");
					break;
				}
			}

			// if data page with freespace not find in current dir page, search next dir page
		    while ((found == 0) && (dir_page_entry->offset != 0)) {
		    	write_page(dir_page, heapfile, dir_page_id);
				int dir_page_id = dir_page_entry->offset / heapfile->page_size;
				bzero(dir_page->data, heapfile->page_size);
				read_page(heapfile, dir_page_id, dir_page);
				dir_page_entry = (Page_entry *)dir_page->data;

				for (i = 1; i < dir_page_capacity; i++) {
					data_page_entry = dir_page_entry + i;
					if ((data_page_entry->freespace > 0) && 
					    (data_page_entry->freespace <= data_page_capacity) &&
					    (data_page_entry->offset > 0)) {

						data_page_id = data_page_entry->offset / heapfile->page_size;
						found = 1;
						// printf("page with freespace found in next dir page\n");
						break;
					}
				}
			}

			// if the data page with free slot found
			if (found == 1) {
				//read data page with data_page_id into memory, insert record
				bzero(data_page->data, heapfile->page_size);
				read_page(heapfile, data_page_id, data_page);
				add_fixed_len_page(data_page, &record);
				// update dir page entry
				data_page_entry->freespace--;

			} 
			// if no data page with free slot found
			if (found == 0) {
				// printf("no data page with free slot (:\n");
				// save the current data page and dir page into disk
				write_page(dir_page, heapfile, dir_page_id);
				// allocate a new data page
				data_page_id = alloc_page(heapfile);
				bzero(data_page->data, heapfile->page_size);
				read_page(heapfile, data_page_id, data_page);
	            // reload dir page, it may different after allocating a new data page
				dir_page_id = ((int)(data_page_id / dir_page_capacity)) * dir_page_capacity;
				bzero(dir_page->data, heapfile->page_size);
				read_page(heapfile, dir_page_id, dir_page);
				dir_page_entry = (Page_entry *)dir_page->data;
				data_page_entry = dir_page_entry + data_page_id % dir_page_capacity;

				add_fixed_len_page(data_page, &record);
				data_page_entry->freespace--;
				
			}
		}
	}

		write_page(data_page, heapfile, data_page_id);
		write_page(dir_page, heapfile, dir_page_id);

		ftime(&t_end);
		end_in_ms = t_end.time * 1000 + t_end.millitm;
		t_diff = end_in_ms - start_in_ms; 

 		free(dir_page->data);
		free(dir_page);
		free(data_page->data);
		free(data_page);
		fclose(csv_fp);
		fclose(heapfile_fp);

		printf("NUMBER OF RECORDS added: %d\n", rec_count);
		printf("TIME: %d ms\n", t_diff);

	return 0;
 }
