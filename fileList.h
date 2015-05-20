/*-------------------------------------------------------------------------------
 * File Organisation, Sorting, and Query Evaluation
 * Name				: Caspersz, Joshua
 * File				: fileL.h
 * Description		: Linked list of file pointers
 -------------------------------------------------------------------------------*/

#ifndef FILELIST_H
#define FILELIST_H
#include "utility.h"
 
typedef struct fileNode{
	FILE * fp;
	struct fileNode * nextFileNode;
	struct fileNode * previousFileNode;
}FileNode;

typedef struct fileL{
	FileNode * fileHeadNode;
	FileNode * fileTailNode;
	unsigned int fileCount;
}FileList;

BOOLEAN initFileList(FileList * fileL);
BOOLEAN addFile(FileList * fileL, FILE * fp);
BOOLEAN removeFile(FileList * fileL, FileNode * fileNode);
void printFileContents(FileList * fileL, Schema schema);
void freeFileNode(FileList * fileL);
#endif