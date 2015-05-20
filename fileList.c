/*-------------------------------------------------------------------------------
 * File Organisation, Sorting, and Query Evaluation
 * Name				: Caspersz, Joshua
 * File				: fileList.c
 * Description		: Linked list of file pointers
 -------------------------------------------------------------------------------*/
#include "fileList.h"

BOOLEAN initFileList(FileList * fileL){
	fileL->fileHeadNode = NULL;
	fileL->fileTailNode = NULL;
	fileL->fileCount = 0;
	return TRUE;
}
/*add file pointer to list*/
BOOLEAN addFile(FileList * fileL, FILE * fp){
	FileNode * new = (FileNode *) malloc(sizeof(FileNode));
	if (!new){
		fprintf(stderr, "Error: Failed to allocate memory for file node\n");
		return FALSE;
	}

	/*set file pointer*/
	new->fp = fp;

	/*tail will be null or tail node*/
	new->previousFileNode = fileL->fileTailNode;
	new->nextFileNode = NULL;

	/*add to end of list*/
	if (fileL->fileTailNode != NULL){
		fileL->fileTailNode->nextFileNode = new;
	}
	/*If first file node*/
	else{
		fileL->fileHeadNode = new;
	}
	fileL->fileTailNode = new;

	/*Increase file count*/
	fileL->fileCount++;

	return TRUE;
}

/*remove file pointer from list and set to null*/
BOOLEAN removeFile(FileList * fileL, FileNode * fileNode){
	/*Remove file node from place in linked list*/
	/*if file to be removed is head, then set head to next node*/
	/*if next node and previous node are null, then head and tail will be set to null*/
	if(fileL->fileHeadNode == fileNode)
		fileL->fileHeadNode = fileNode->nextFileNode;
	else /*remove file node from previous nodes link*/
		fileNode->previousFileNode->nextFileNode = fileNode->nextFileNode;

	if(fileL->fileTailNode == fileNode)
		fileL->fileTailNode = fileNode->previousFileNode;
	else /*remove file node from next node's previous link*/
		fileNode->nextFileNode->previousFileNode = fileNode->previousFileNode;

	/*close temporary file*/
	fclose(fileNode->fp);

	free(fileNode);
	fileL->fileCount--;

	return TRUE;
}

/*Debug file printing*/
void printFileContents(FileList * fileL, Schema schema){
	FileNode * current = fileL->fileHeadNode;
	int fileCount = 0;
	while (current != NULL){
		FILE * initTempFP = current->fp;
		Record record;
		printf("File:%i\n",fileCount);
		while(fread(&record, sizeof(Record), 1, initTempFP) == 1){	
			if(schema == CHARACTER)
				printf("\t%s %i %i %i %i\n",record.field.character.cname, 
				record.field.character.team, record.field.character.level,
				record.field.character.cid, record.GuildID);
			else
				printf("\t%i %s\n",record.GuildID, record.field.g_name);
		}
		fileCount++;
		rewind(current->fp);
		current = current->nextFileNode;
	}
}

void freeFileNode(FileList * fileL){
	FileNode * currentFile = fileL->fileHeadNode;
	FileNode * nextFile;
	while(currentFile != NULL){
		nextFile = currentFile->nextFileNode;
		free(currentFile);
		currentFile = nextFile;
	}
}