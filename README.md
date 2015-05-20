File Organisation, Sorting, and Query Evaluation


Build the program using the command
make all


To run the external merge sort execute the following command:

./mergesort 'pagesize' 'buffers' 'path'


To run the sort merge execute one of the following commands:

./sortmerge [-o] 'pagesize' 'buffers' 'path'/characters 'path'/guilds

./sortmerge [-o] 'pagesize' 'buffers' 'path'/guilds 'path'/characters


Where:
'pagesize' is the number of records that fit in a page

'buffers' the number of buffer pages that are available in memory for processing purposes

'path' is the name of the file to be sorted



Explantation of external merge sort implementation:

1. Read in buffer size*page size number of records from the file at a time into a union of structs

2. Sort the records in memory using quicksort then switch to insertion sort when the number of records to be sorted is less than 9. Write these records to a temporary file and add the file pointer to a linked list.

3. Repeat steps 1 & 2 (a run) until the file has been completely processed (finished 1 pass).

4. While the number of files in the linked list is greater than 1, merge the files as follows:

5. For each run, merge buffersize-1 files. First read in pagesize
number of records from each file into a buffer and add a pointer of each read in record to a minheap ordered by GuildID.

6. Pop a record pointer off the minheap and move the corresponding record with the lowest GuildID to the output buffer.

7. If the output buffer is now full (has pagesize number of records) write out to the run’s temporary file and clear the output buffer.

8. If any buffer (besides the output buffer) is now empty, read in pagesize
number of records from its relevant temporary file from the previous pass.

9. Repeat steps 6-8 until all files for the run have been fully processed.

10. If there are still files to process from the previous pass commence a run.(From step 5)

11. Once there is no more files to process for a pass, set the linked list of output file pointers for the run to be the linked list of input file pointers for next run.

12. Repeat steps 4-11 until there is only 1 temporary binary file left. (each loop of steps is 1 pass)

13. Write out this temporary binary file to a text file.