#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>

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
	if ((heapfile_fp = fopen(argv[1], "r")) == NULL) {
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
	heapfile->file_ptr = heapfile_fp;
	heapfile->page_size = page_size;



	return 0;
 }
