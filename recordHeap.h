/*-------------------------------------------------------------------------------
 * File Organisation, Sorting, and Query Evaluation
 * Name				: Caspersz, Joshua
 * File				: recordHeap.h
 * Description		: Priority queue implemented using heap for storing records
 -------------------------------------------------------------------------------*/

#ifndef RECORDHEAP_H
#define RECORDHEAP_H
#include "utility.h"
#include "buffer.h"

typedef struct recordIndex{
	/*the buffer the record is stored in*/
	Buffer * buff;
	/*the index of the array of records for the page*/
	unsigned pgIndex;
	/*pointer to the record's guildID*/
	unsigned * guildID;
}RecordIndex;

/*Record min heap, priority queue on guildID, contains:
Capacity of heap
Actual record count of heap
Priority queue of record indexes (in the form of a heap)
pq is a priority queue implemented by a heap*/

typedef struct rHeap{
	unsigned capacity;
	unsigned count;
	RecordIndex * heap;
}RecordHeap;

BOOLEAN initHeap(RecordHeap * rHeap, unsigned heapSize);
BOOLEAN addToHeap(RecordHeap * rHeap, Buffer * buffer, unsigned pageIndex);
RecordIndex removeMinHeap(RecordHeap * rHeap);
void freeHeap(RecordHeap * rHeap);

#endif