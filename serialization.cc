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
 */
void fixed_len_write(Record *record, void *buf) {
	//std::vector<V> Record = {'aaaaa aaaaa', 'bbbbb bbbbb', ... 'zzzzz zzzzz'};
	//*buf = 'aaaaaaaaaabbbbbbbbbb ... zzzzzzzzzz'
  char* buffer = (char *)buf;
	int i = 0;
	Record::iterator it;
	for(it = record->begin(); it != record->end(); ++it) {
			printf("%s\n", *it);
			strncpy(buffer + i * 10, *it, 10);
			i++;
	}
}

/**
 * Deserialize 'size' bytes from the buffer 'buf' and
 * store the record in 'record'.
 * Input: fixed_len_read('aaaaabbbbbccccc', 1000, {});
 * Output: {'aaaaa', 'bbbbb', 'ccccc'}
 */
void fixed_len_read(void *buf, int size, Record *record) { //malloc?
	/**
	for (int i=0; i < size; i+10){
		char *temp = buf[i, i+9];
		record.push_back(temp);
	}
	*/
}
int main(int argc, char *argv[]){

	char *a = (char *)malloc(11);
	strncpy (a, "aaaaaaaaaa", 10);
	char *b = (char *)malloc(11);
	strncpy (b, "bbbbbbbbbb", 10);
	char *c = (char *)malloc(11);
	strncpy (c, "cccccccccc", 10);

	// Record record;
	// record.push_back(a);
	// record.push_back(b);
	// record.push_back(c);
	// Record::iterator it;
	// for(it = record.begin(); it != record.end(); ++it) {
	// 		printf("%s\n", *it);
	// }
	void *buf;
	Record *r = new Record();
	r->push_back(a);
	r->push_back(b);
	r->push_back(c);
	fixed_len_write(r, buf);





// 	std::cout << "myvector contains:";
// for (std::vector<V>::iterator it = record.begin() ; it != record.end(); ++it)
// 	std::cout << ' ' << *it;
// std::cout << '\n';
	return 0;


// 	// constructors used in the same order as described above:
// std::vector<int> first;                                // empty vector of ints
// std::vector<int> second (4,100);                       // four ints with value 100
// std::vector<int> third (second.begin(),second.end());  // iterating through second
// std::vector<int> fourth (third);                       // a copy of third
//
//
// std::vector<V> record (4, "hello");
//
//
// // the iterator constructor can also be used to construct from arrays:
// int myints[] = {16,2,77,29};
// std::vector<int> fifth (myints, myints + sizeof(myints) / sizeof(int) );
//
// std::cout << "The contents of fifth are:";
// for (std::vector<int>::iterator it = fifth.begin(); it != fifth.end(); ++it)
// 	std::cout << ' ' << *it;
// std::cout << '\n';
//
// return 0;
}
