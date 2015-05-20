/*-------------------------------------------------------------------------------
 * File Organisation, Sorting, and Query Evaluation
 * Name				: Caspersz, Joshua
 * File				: buffer.h
 * Description		: Buffer definition
 -------------------------------------------------------------------------------*/

#ifndef BUFFER_H
#define BUFFER_H
#include "utility.h"
#include "fileList.h"
 
/*Each buffer node has:
-a page array containing page-size number of records
-page array record count
-file node: output file for output buffer, else input file*/
typedef struct buffer{
	Record * pageRecords;
	unsigned recordCount;
	FileNode * fileNode;
}Buffer;


BOOLEAN initBuffer(Buffer * buffer, int pageSize);
BOOLEAN addRecord(Buffer * buffer, Record pageRecord, int pageSize, int pageIndex);
void removeRecord(Buffer * buffer, int pageIndex);
void clearPage(Buffer * buffer, int pageSize);
void freeBuffer(Buffer * buffer);

#endif