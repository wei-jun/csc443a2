#include <vector>
typedef const char* V;
typedef std::vector<V> Record;

/**
 * Compute the number of bytes required to serialize record.
 */
int fixed_len_sizeof(Record *record) {

}

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf);

/**
 * Deserialize 'size' bytes from the buffer 'buf' and
 * store the record in 'record'.
 */
void fixed_len_read(void *buf, int size, Record *record);
