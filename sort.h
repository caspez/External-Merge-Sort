/*-------------------------------------------------------------------------------
 * File Organisation, Sorting, and Query Evaluation
 * Name				: Caspersz, Joshua
 * File				: sort.h
 * Description		: External merge sort functions
 -------------------------------------------------------------------------------*/

#ifndef SORT_H
#define SORT_H

#include "utility.h"
#include "fileList.h"
#include "recordHeap.h"
#include "buffer.h"

#define QUICK_SORT_CUTOFF 9

BOOLEAN externalMergeSort(FILE ** sorted, int pagesize, int buffers, char * path, int * passes, int * runs, Schema * schema);
void quickSort(Record * records, int left, int right);
void insertionSort(Record * records, int recordCount);
BOOLEAN initialRun(char * filePath, int bufferMemoryCount, Schema * schema, FileList * fileL, int * totalRecordCount);
BOOLEAN mergeRuns(FILE ** sorted, int pagesize, int availableBuffers, FileList currentFiles, int * passes, int * runs, int totalRecordCount);
#endif