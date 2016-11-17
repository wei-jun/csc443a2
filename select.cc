#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include <vector>

#include "library.h"

/*
 * Print out all records in a heap file
 *$ select <heapfile> <attribute_id> <start> <end> <page_size>
 */

 int main(int argc, char* argv[])
 {
 	// input sanity check
 	if (argc != 6) {
 		fprintf(stderr, "Usage: %s <heapfile> <attribute_id> <start> <end> <page_size>\n", argv[0]);
        exit(1);
	}
	FILE *heapfile_fp;
	if ((heapfile_fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "cannot read file %s\n", argv[1]);
		exit(1);
	}
	int attribute_id = atoi(argv[2]);
	char *start = argv[3];
	char *end = argv[4];
	int page_size = atoi(argv[5]);
  printf("Running query: Select Attribute:%d from %s where start is %s and end is %s.\n", attribute_id, argv[1],start, end);
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
  int qualified_count = 0;
	ftime(&t_start);
	start_in_ms = t_start.time * 1000 + t_start.millitm;

	printf("just before 'while-loop'\n");
	while (iterator.hasNext()) {
		record = iterator.next();
		fixed_len_write(&record, buf);
		rec_count++;


		char temp[ATTR_LEN + 1];
		temp[ATTR_LEN] = '\0';

    //printf("Comparing attribute: %s\n", temp);
		strncpy(temp, (char *)buf + attribute_id * ATTR_LEN, ATTR_LEN);
		if ((strcmp(temp, start) >= 0) && (strcmp(temp, end) <= 0)) {
			printf("Qualified: %s\n", temp);
      qualified_count++;
		}
	}

	ftime(&t_end);
	end_in_ms = t_end.time * 1000 + t_end.millitm;
	t_diff = end_in_ms - start_in_ms;

	free(heapfile);
	fclose(heapfile_fp);

	//printf("NUMBER OF RECORDS: %d\n", rec_count);
  printf("Total Records: %d\n", rec_count);
  printf("Qualified Records: %d\n", qualified_count);
  printf("TIME: %d\n", t_diff);

 	return 0;
 }
