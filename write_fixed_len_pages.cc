#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>

#include "library.h"

/**
 *Load as many records from a comma separated file to fill up a page,  and append 
 *the  age to a file. Repeat until all the records in the CSV files are written to 
 *the page file. Your program should follow the following syntax, and produce the 
 *output containing record count, page count, and time took, similar to as follows:
 *$ write_fixed_len_pages <csv_file> <page_file> <page_size>
 *NUMBER OF RECORDS: 1000
 *NUMBER OF PAGES: 32
 *TIME: 43 milliseconds
 **/

 int main(int argc, char* argv[])
 {
    // input sanity check
 	if (argc != 4) {
		printf("usage: write_fixed_len_pages <csv_file> <page_file> <page_size>\n");
		exit(-1);
	}
	FILE *csv_fp;
	if ((csv_fp = fopen(argv[1], "r")) == NULL) {
		printf("cannot read file %s\n", argv[1]);
		exit(-1);
	}	
	int page_size = atoi(argv[3]);
	if (page_size <= 0) {
		printf("page_size should be positive.\n");
		exit(-1);
	}

	FILE *page_fp = fopen(argv[2], "w");
	char line[MAXLINE];  // line in csv file
	char row[MAXLINE];   // row in page
	bzero(line, MAXLINE);
	bzero(row, MAXLINE);
	Record *record;
	char *slot_ptr;
	Page *page;
	int slot_size = 1 + ATTRS_SIZE * ATTR_LEN;
	int page_capacity = page_size / slot_size;
	page_size = (page_size / slot_size) * slot_size;
	// init_fixed_len_page(page, page_size, slot_size);
	page = (Page *)malloc(sizeof(Page));
	page->page_size = page_size;
	page->slot_size = slot_size;
	page->data = (void *)malloc((page_size / slot_size) * slot_size);
	bzero(page->data, (page_size / slot_size) * slot_size);
	
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
		//printf("first value is: %s\n", curr_attr);
		int i = 0;
		// remove common "," from the line and save into row
		while (curr_attr) {
			strncpy(row + i * ATTR_LEN, curr_attr, ATTR_LEN);
			curr_attr = strtok(NULL, ",");
			i++;			
		}
		//printf("row read is: %s\n\n", row);
		
		/*
		// use the row to construct a record
		fixed_len_read(row, ATTRS_SIZE * ATTR_LEN, record);

		// write the record into a page
		write_fixed_len_page(page, j, record);
		*/
		
		// write the row into a page
		slot_ptr = (char *)page->data + j * slot_size;
		strncpy(slot_ptr, "1", 1);

		// printf("slot flag is: %s \n", slot_ptr);

		memcpy(slot_ptr + 1, row, slot_size - 1);

		// printf("slot content is: %s\n\n", slot_ptr);

		bzero(line, MAXLINE);
	    bzero(row, MAXLINE);
		rec_count++;
		j++;
		// if the page is full, write to page file		
		if (j == page_capacity) {
			fwrite(page->data, page_size, 1, page_fp);
			fflush(page_fp);
			bzero(page->data, page_size);
			page_count++;
			j = 0;
		}		
	}
	// write the last not-full page if applicable
	if (j != 0) {
		fwrite(page->data, page_size, 1, page_fp);
		page_count++;
	}		

	ftime(&t_end);
	end_in_ms = t_end.time * 1000 + t_end.millitm;
	t_diff = end_in_ms - start_in_ms; 

	free(page->data);
	free(page);
	fclose(csv_fp);
	fclose(page_fp);

	printf("NUMBER OF RECORDS: %d\n", rec_count);
	printf("NUMBER OF PAGES: %d\n", page_count);
	printf("TIME: %d ms\n", t_diff);

 	return 0;
 }