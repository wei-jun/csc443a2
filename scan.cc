#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include <vector>

#include "library.h"

/*
 * Print out all records in a heap file
 *$ scan <heapfile> <page_size>
 */

 int main(int argc, char* argv[])
 {
 	// input sanity check
 	if (argc != 3) {
 		fprintf(stderr, "Usage: %s <heapfile> <page_size>\n", argv[0]);
        exit(1);
	}
	FILE *heapfile_fp;
	if ((heapfile_fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "cannot read file %s\n", argv[1]);
		exit(1);
	}	
	int page_size = atoi(argv[2]);
	if (page_size <= 0) {
		fprintf(stderr, "page_size should be greater than %d.\n", NUM_ATTRS * ATTR_LEN);
		exit(1);
	}

	Heapfile *heapfile = (Heapfile *)malloc(sizeof(Heapfile));
	init_heapfile(heapfile, page_size, heapfile_fp);
	RecordIterator iterator(heapfile);
	Record record;
	char buf[sizeof(Slot)];
	bzero(buf, sizeof(Slot));
	char attr_value[ATTR_LEN + 1];
	bzero(attr_value, ATTR_LEN + 1);

	int rec_count = 0;
	struct timeb t_start, t_end;
	long start_in_ms, end_in_ms;
	int t_diff = 0;
	ftime(&t_start);
	start_in_ms = t_start.time * 1000 + t_start.millitm;

	while (iterator.hasNext()) {
		record = iterator.next();
		fixed_len_write(&record, buf);
		rec_count++;
        
        /* for select
		char temp[ATTR_LEN + 1];
		temp[ATTR_LEN] = '\0';

		strncpy(temp, (char *)buf + attribute_id * ATTR_LEN, ATTR_LEN)
		if ((strcmp(temp, start) >= 0) && (strcmp(temp, end) <= 0)) {
			printf("%s \n", temp);
		}
		*/

		// print the record as csv format
		int j;
		for (j=0; j<(NUM_ATTRS - 1); j++) {
			strncpy(attr_value, buf + j * ATTR_LEN, ATTR_LEN);
			attr_value[ATTR_LEN] = '\0';
			printf("%s,", attr_value);
			bzero(attr_value, ATTR_LEN + 1);
		}
		// print the last attribute value
		strncpy(attr_value, buf + j * ATTR_LEN, ATTR_LEN);
		attr_value[ATTR_LEN] = '\0';
		printf("%s\n\n", attr_value);
		bzero(attr_value, ATTR_LEN + 1);
	}

	ftime(&t_end);
	end_in_ms = t_end.time * 1000 + t_end.millitm;
	t_diff = end_in_ms - start_in_ms;
	
	free(heapfile);
	fclose(heapfile_fp);

	//printf("NUMBER OF RECORDS: %d\n", rec_count);
	printf("TIME: %d\n", t_diff);

 	return 0;
 }










