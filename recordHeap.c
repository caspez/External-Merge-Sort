/*-------------------------------------------------------------------------------
 * File Organisation, Sorting, and Query Evaluation
 * Name				: Caspersz, Joshua
 * File				: recordHeap.c
 * Description		: Priority queue implemented using heap for storing records
 *                    Based on this algorithm:
 *                    http://en.wikibooks.org/wiki/Data_Structures/Min_and_Max_Heaps
 -------------------------------------------------------------------------------*/
#include "recordHeap.h"

/*Initialises heap for use*/
BOOLEAN initHeap(RecordHeap * rHeap, unsigned int heapSize){
	rHeap->count = 0;
	rHeap->capacity = heapSize;
	if ((rHeap->heap = calloc(heapSize, sizeof(RecordIndex))) == NULL) {
		fprintf(stderr, "Error: Failed to allocate memory for heap\n");
		return FALSE;
	}
	return TRUE;
}

void swapRecordIndex(RecordIndex * r1, RecordIndex * r2){
	RecordIndex tmpR;
	tmpR = *r1;
	*r1 = *r2;
	*r2 = tmpR;
}

/*Adds record to heap*/
BOOLEAN addToHeap(RecordHeap * rHeap, Buffer * buffer, unsigned pageIndex){
	int i = rHeap->count;
	/*RecordIndex recordIndex;*/

	if (rHeap->count >= rHeap->capacity){
		fprintf(stderr, "Error: heap is full\n");
		return FALSE;
	}

	rHeap->heap[i].buff = buffer; 
	rHeap->heap[i].pgIndex = pageIndex;
	rHeap->heap[i].guildID = &buffer->pageRecords[pageIndex].GuildID;

	/*insert record index at end of heap*/
	/*rHeap->heap[i] = recordIndex;*/

	/*Move into correct position*/
	/*That is, swap with parent as long as guildID is smaller*/
	/*get index of record*/
	/*while(*rHeap->heap[(i-1)/2].guildID < *recordIndex.guildID){
		rHeap->heap[i]  = rHeap->heap[(i-1)/2];
		i = (i-1) / 2;
	}*/
	while(i > 0){
		if(*rHeap->heap[(i-1)/2].guildID <= *rHeap->heap[i].guildID)
			break;
		swapRecordIndex(&rHeap->heap[(i-1)/2], &rHeap->heap[i]);
		i = (i-1) / 2;
	}

	/*rHeap->heap[i] = recordIndex;*/

	rHeap->count++;

	return TRUE;
}

/*Removes record with smallest guildID from heap and returns it*/
RecordIndex removeMinHeap(RecordHeap * rHeap){
	int i = 0;

	/*Root of heap has the smallest guildID*/
	RecordIndex minRecord = rHeap->heap[0];

	/*debugging check to see if min record is actually smallest*/
	/*RecordIndex nullRecord;
	nullRecord.guildID  =0;
	for(i = 1; i<rHeap->count; i++){
		if (*rHeap->heap[0].guildID > *rHeap->heap[i].guildID){
			fprintf(stderr, "Error: Invalid min value for heap\n");
			return nullRecord;
		}
	}*/

	if(rHeap->count <= 0){
		fprintf(stderr, "Error: Attempting to get min when heap is empty\n");
		return minRecord;
	}

	/*Set last record of heap as root and decrease heap counter*/
	rHeap->heap[0] = rHeap->heap[--rHeap->count];
	/*reorder heap*/
	while(i < rHeap->count){
		int min=i;
		/*left child is index 2*i+1 and right child is at index (2*i +2)*/
		if((2*i+1)< rHeap->count && *rHeap->heap[2*i+1].guildID < *rHeap->heap[min].guildID)
			min=2*i+1;
		if((2*i+2)< rHeap->count && *rHeap->heap[2*i+2].guildID < *rHeap->heap[min].guildID) 
			min=2*i+2;
		if(min!=i){
			/*swap records*/
			swapRecordIndex(&rHeap->heap[i], &rHeap->heap[min]);
			i=min;
		}
		else 
			break;
	}
	return minRecord;
}

void freeHeap(RecordHeap * rHeap){
	free(rHeap->heap);
}