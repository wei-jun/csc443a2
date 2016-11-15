#csc443 assignment2
Relational data Model on disk

zhao & zeng

1. Generate a random csv file
python mkcsv.py csvfile 10


2. Convert from csvfile to pagefile
./write_fixed_len_pages csvfile pagefile 1024

3. Read the pagefile
./read_fixed_len_page pagefile 1024


4. Convert from csvfile to heapfile
./csv2heapfile csvfile heapfile 1024

5. Scan the heapfile
./scan heapfile 1024

6. Convert from csvfile to colstorefile
