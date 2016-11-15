#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>

typedef char* V;
typedef std::vector<V> Record;

/**
 * Compute the number of bytes required to serialize record.
 */
int fixed_len_sizeof(Record *record) {
	int len = 0;
	std::vector<V>::iterator it;
	for(it = record->begin(); it != record->end(); ++it) {
		len += strlen(*it);
	}
	return len;
}

/**
 * Serialize_Record. Serialize the record to a byte array to be stored in buf.
 * std::vector<V> Record = {'aaaaa aaaaa', 'bbbbb bbbbb', ... 'zzzzz zzzzz'};
 * void *buf = 'aaaaaaaaaabbbbbbbbbb ... zzzzzzzzzz'
 */
void fixed_len_write(Record *record, void *buf) {
  //char* buffer;// = (char *)buf;
	//char* buffer = static_cast<char*>(buf);
	int i = 0;
	Record::iterator it;
	for(it = record->begin(); it != record->end(); ++it) {
			printf("%s\n", *it);
			strncpy((char *)buf+i*10, *it, 10);
			i++;
	}
	printf("Buffer: %s\n", (char *)buf);
}

/**
 * Deserialize 'size' bytes from the buffer 'buf' and
 * store the record in 'record'.
 * Input: fixed_len_read('aaaaabbbbbccccc', 1000, {});
 * Output: {'aaaaa', 'bbbbb', 'ccccc'}
 */
void fixed_len_read(void *buf, int size, Record *record) {
	char* charptr = (char*) buf;
	char* temp = (char *)malloc(11); //temporily store each and every 10 bits words

	for (int i=0; i < size; i+=10){
		strncpy(temp, charptr+i, 10);
		printf("%s\n", temp);
		record->push_back(temp);
	}
	for (Record::const_iterator i = record->begin(); i != path->end(); ++i)
    std::cout << *i << ' ';
}
int main(int argc, char *argv[]){

	char *a = (char *)malloc(11);
	strncpy (a, "aaaaaaaaaa", 10);
	char *b = (char *)malloc(11);
	strncpy (b, "bbbbbbbbbb", 10);
	char *c = (char *)malloc(11);
	strncpy (c, "cccccccccc", 10);

	char *d = (char *)malloc(11);
	strncpy (d, "dddddddddd", 10);
	char *e = (char *)malloc(11);
	strncpy (e, "eeeeeeeeee", 10);
	char *f = (char *)malloc(11);
	strncpy (f, "ffffffffff", 10);


	void *buf;
	Record *r = new Record();
	r->push_back(a);
	r->push_back(b);
	r->push_back(c);
	r->push_back(d);
	r->push_back(e);
	r->push_back(f);
	fixed_len_write(r, buf);

  // memcpy(buf, "abc", 4);
	// printf("%s\n", buf);
	// char *a2 = (char *)malloc(31);
	// strncpy (a2, "zzzzzzzzzzxxxxxxxxxxyyyyyyyyyy", 30);
	// fixed_len_read(a2, 30, r);

	return 0;
}
