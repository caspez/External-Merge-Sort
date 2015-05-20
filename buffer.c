/*-------------------------------------------------------------------------------
 * File Organisation, Sorting, and Query Evaluation
 * Name				: Caspersz, Joshua
 * File				: buffer.c
 * Description		: Buffer definition
 -------------------------------------------------------------------------------*/
#include "buffer.h"

/*initialise buffer*/
BOOLEAN initBuffer(Buffer * buffer, int pageSize){
	buffer->pageRecords = calloc(pageSize, sizeof(Record));
	if (!buffer->pageRecords){
		fprintf(stderr, "Error: Failed to allocate page array\n");
		return FALSE;
	}

	buffer->recordCount = 0;
	buffer->fileNode = NULL;

	return TRUE;
}

/*adds record to page in buffer*/
/*returns record index, where index is position in pageRecords array*/
BOOLEAN addRecord(Buffer * buffer, Record pageRecord, int pageSize, int pageIndex){
	if(buffer->recordCount >= pageSize){
		fprintf(stderr, "Error: Page is full\n");
		return FALSE;
	}
	/*insert element at pageindex*/
	if(pageIndex < pageSize){
		buffer->pageRecords[pageIndex] = pageRecord;
		buffer->recordCount++;
		return TRUE;
	}
	
	/*If reach here, error occurred*/
	fprintf(stderr, "Error: Invalid page index: %i\n",pageIndex);
	return FALSE;
}

/*remove record from buffer*/
void removeRecord(Buffer * buffer, int pageIndex){
	/*Empty record has guildID=0 */
	buffer->pageRecords[pageIndex].GuildID = 0;
	buffer->recordCount--;
}

/*clear page, that is remove all records from buffer*/
/*Will be used when the whole page is written to file
from the output buffer*/
void clearPage(Buffer * buffer, int pageSize){
	int i;
	for(i = 0; i < pageSize; i++){
		buffer->pageRecords[i].GuildID = 0;
		/*while technically the rest of record is still there, it will
		be treated as empty, since guildID=0*/
		/*The next record will be inserted in place of the old record*/
	}
	buffer->recordCount = 0;
}

/*free page allocation for buffer*/
void freeBuffer(Buffer * buffer){
	free(buffer->pageRecords);
}