#include <vector>
typedef char* V;
typedef std::vector<V> Record;

/**
 * Compute the number of bytes required to serialize record.
 */
int fixed_len_sizeof(Record *record) {
	int i = sizeof(record);
	return i;
}

/**
 * Serialize_Record. Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf) {
	//std::vector<V> Record = {'aaaaa aaaaa', 'bbbbb bbbbb', ... 'zzzzz zzzzz'};
	//*buf = 'aaaaaaaaaabbbbbbbbbb ... zzzzzzzzzz'

	char *buf = (char*)buf;
	std::vector<V>::iterator it;
	for(it = Record.begin(); it != Record.end(); ++it) {
 	    *buf = *it;
	    *buf += 10;
	}	
}

/**
 * Deserialize 'size' bytes from the buffer 'buf' and
 * store the record in 'record'.
 * Input: fixed_len_read('aaaaabbbbbccccc', 1000, {});
 * Output: {'aaaaa', 'bbbbb', 'ccccc'}
 */
void fixed_len_read(void *buf, int size, Record *record) {
	for (int i=0; i < size; i+10){
		char *temp = buf[i, i+9];
		Record.push_back(temp);
	}
}
