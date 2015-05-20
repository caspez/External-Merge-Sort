/*-------------------------------------------------------------------------------
* COSC2406/2407 Database Systems
* 2014 Semester 1 Assignment 2:File Organisation, Sorting, and Query Evaluation
* Name				: Caspersz, Joshua
* File				: sort.c
* Description		: External merge sort functions
-------------------------------------------------------------------------------*/
#include "sort.h"

/*Perform external merge sort on file*/
/*Returns sorted file as a binary file containing record structs*/
/*Assumes file is either in the character or guilds schema*/
BOOLEAN externalMergeSort(FILE ** sorted, int pagesize, int buffers, char * path, int * passes, int * runs, Schema * schema){
	FileList initialFileList;
	int totalRecordCount = 0;
	initFileList(&initialFileList);

	printf("Commencing sort merge on file '%s'\n",path);
	if(initialRun(path, (pagesize * buffers), schema, &initialFileList, &totalRecordCount) == FALSE)
		return TRUE;
	*passes = 1;
	*runs += initialFileList.fileCount;
	printf("Pass %i resulted in %i runs\n",*passes,*runs);

	/*printFileContents(&initialFileList, *schema);*/

	mergeRuns(sorted, pagesize, buffers, initialFileList, passes, runs, totalRecordCount);
	rewind(*sorted);

	return TRUE;

}

/*Initial run, read in given file and save sorted blocks to temporary files*/
BOOLEAN initialRun(char * filePath, int bufferMemoryCount, Schema * schema, FileList * fileL, int * totalRecordCount){
	FILE * initialFP;
	char line[LINE_LENGTH];
	char * tokenPtr;
	int i;

/*Open path file*/
	if((initialFP = fopen(filePath,"r")) == NULL){
		fprintf(stderr, "Error: Failed to open file %s\n",filePath);
		return FALSE;
	}

/*Determine and set type of file data*/
/*Get first line of file and count number of ','*/
	fgets(line, LINE_LENGTH, initialFP);
/*rewind to start of file for future use*/
	rewind(initialFP);
/*Determine first occurrence of ',' in line*/
	tokenPtr = strchr(line,',');
/*Determine if there is a second occurrence of ',' in line*/
	tokenPtr = strchr(tokenPtr+1,',');
/*if NULL then file contains guild records*/
/*(since there is only 1 ',' for every guild file line)*/
	if(tokenPtr == NULL)
		*schema = GUILD;
/*else then file contains character records*/
	else
		*schema = CHARACTER;

/*While there is still more of the file to read*/
	while (line[0] != '\0'){
		FILE * tempFP;
		int recordCount = bufferMemoryCount;
		Record * records = calloc(bufferMemoryCount, sizeof(Record));
		if (!records){
			fprintf(stderr, "Error: Failed to allocate memory for records\n");
			return FALSE;
		}

		for(i = 0; i < bufferMemoryCount; i++){
			char * tempString = NULL;
/*If reached end of file*/
			if(fgets(line, LINE_LENGTH, initialFP) == NULL){
/*set record size to index reached*/
				recordCount = i;
				line[0] = '\0';
				break;
			}

			if(*schema == CHARACTER){
				strcpy(records[i].field.character.cname,strtok(line,","));

				tempString = strtok(NULL,",");
				records[i].field.character.team = (unsigned short int) strtoul(tempString, NULL, 0);

				tempString = strtok(NULL,",");
				records[i].field.character.level = (unsigned short int) strtoul(tempString, NULL, 0);

				tempString = strtok(NULL,",");
				records[i].field.character.cid = (unsigned int) strtoul(tempString, NULL, 0);

				tempString = strtok(NULL,",");
				records[i].GuildID = (unsigned int) strtoul(tempString, NULL, 0);
			}
			else{ /*If GUILD schema*/
				int lineLen = strlen(line);
				/*Remove new line from end of string*/
				if (lineLen > 0 && line[lineLen-1] == '\n')
					line[lineLen-1] = '\0';

				tempString = strtok(line,",");
				records[i].GuildID = (unsigned int) strtoul(tempString, NULL, 0);

				strcpy(records[i].field.g_name,strtok(NULL,","));
			}
			*totalRecordCount = *totalRecordCount +1;
		}

		/*Sort the blocks*/
		quickSort(records, 0, recordCount-1);
		/*insertionSort(records, recordCount);*/
	/*Write out buffers run of B blocks to temp files*/
	/*create temporary file*/
		if((tempFP = tmpfile()) == NULL){
			fprintf(stderr, "Error: Failed to create temporary file\n");
			return FALSE;
		}

/*Write array of records to binary file*/
		fwrite(records, sizeof(Record), recordCount, tempFP);
/*no need to loop through records array*/
/*for (i = 0; i < recordCount; i++){
fwrite(&records[i], sizeof(Record), 1, tempFP);
}*/

		/*Rewind temp file pointer for later merging*/
		rewind(tempFP);
		/*add file pointer to file list*/
		addFile(fileL, tempFP);

		free(records);
	}
	fclose(initialFP);
	return TRUE;
}

/*Uses insertion sort to sort an array of records*/
void insertionSort(Record * records, int recordCount){
	int i, j;
	Record tempRecord;
	for(i=1; i<recordCount; i++){
		tempRecord = records[i];
		j = i - 1;
		while(j >= 0 && (tempRecord.GuildID < records[j].GuildID)){
			records[j+1] = records[j];
			j--;
		}
		records[j+1] = tempRecord;
	}
}

/*Perform Quick Sort on an array of strings*/
/*(Based on my assignment 1 for Algorithm and Analysis Summer 2014)*/
void quickSort(Record * records, int left, int right) {
	Record piv;
	int i = left, j = right;

	/*Switch to insertion sort if the size of the array being sorted 
	is less than the quick sort cut-off threshold*/
	if(right-left < QUICK_SORT_CUTOFF){
		insertionSort(records, right-left+1);
	}
	/*Perform quick sort*/
	/*set piv*/
	if (right - left < 1)
		return;
	piv = records[(left + right) / 2];
	while (i <= j) {
		while ((i < right) && (records[i].GuildID < piv.GuildID)){
			i++;
		}
		while ((j > left) && (records[j].GuildID > piv.GuildID)){
			j--;
		}
		if (i <= j) {
			/*Swap the records*/
			Record tmp = records[i];
			records[i] = records[j];
			records[j] = tmp;

			i++;
			j--;
		}
	}

	if (left < j)
		quickSort(records, left, j);
	if (i < right)
		quickSort(records, i, right);
	
}


BOOLEAN mergeRuns(FILE ** sorted, int pagesize, int availableBuffers, FileList currentFiles, int * passes, int * runs, int totalRecordCount){
	RecordHeap rHeap;
	Buffer * bufferNodes;
	int bufferIndex;

/*Determine max files that can be merged in a run*/
/*That is available buffers -1 for the output buffer*/
/*This is also the output buffer indes*/
	int outputBuffIndex = availableBuffers -1;

/*Buffer array (of buffer nodes) where size of which is the number
of buffers available to store in memory*/
	bufferNodes = calloc(availableBuffers, sizeof(Buffer));
	if(!bufferNodes){
		fprintf(stderr, "Error: Failed to allocate buffer array\n");
		return FALSE;
	}

/*Allocate memory for record arrays for each buffer*/
	for(bufferIndex = 0; bufferIndex < availableBuffers; bufferIndex++){
		initBuffer(&bufferNodes[bufferIndex], pagesize);
	}

/*Initialise priority queue
It's size is the amount of files that can be merged in a run*/
/*outputBuffIndex is the last index in the buffer array*/
	if(initHeap(&rHeap, outputBuffIndex * pagesize) == FALSE){
		return FALSE;
	}

/*while more merging required, (more than 1 temporary file)*/
/*go through a pass*/
	while(currentFiles.fileCount > 1){
		int runCount = 0;
		/*Define first file to be the start of the file linked list*/
		FileNode * firstFileForRun = currentFiles.fileHeadNode;

		/*Run file list, is the files to be merged in the next pass*/
		FileList runFileList;/*= calloc(1, sizeof(FileList));*/

		float runsInPassFloat = ((float)currentFiles.fileCount/(float)(availableBuffers-1));
		int runsInPass = ceil(runsInPassFloat);

		initFileList(&runFileList);


/*while still merging required for pass*/
/*go through a run*/
		while(runCount < runsInPass){
			int buffersInUse = 0;
			int bufferIndex = 0;
			int init = 0;
			FileNode * currentRunFile = firstFileForRun;
			FILE * outputFile;
			
/*create new temp file for merge run, written to when output buffer is full*/
			if((outputFile = tmpfile()) == NULL){
				fprintf(stderr, "Error: Failed to create output temporary file for run\n");
				return FALSE;
			}

/*add file pointer to the file list for the next pass*/
			addFile(&runFileList,outputFile);

/*Read in pages from current files to buffers*/
			for(bufferIndex = 0; bufferIndex < outputBuffIndex; bufferIndex++){
				int recordPageIndex;
/*fill buffer with records from file*/
				if(currentRunFile->fp != NULL){
					for(recordPageIndex = 0; recordPageIndex < pagesize; recordPageIndex++){
						/*read in record*/
						Record record;
						if(fread(&record, sizeof(Record), 1, currentRunFile->fp) == 1){
							/*add record to page (records array)*/
							init++;
							if(addRecord(&bufferNodes[bufferIndex], record, pagesize, recordPageIndex) == FALSE)
								return FALSE;
		/*add record index to heap*/
							if(addToHeap(&rHeap, &bufferNodes[bufferIndex], recordPageIndex) == FALSE)
								return FALSE;
						}
		/*else reached file end*/
						else{
							/*temp file will be automatically deleted by the system*/
							fclose(currentRunFile->fp);
							currentRunFile->fp = NULL;
							/*removeFile(currentFiles, currentRunFile);*/
							/*add blank records*/
							/*int blankRecordCount;
							for(blankRecordCount = recordCount; blankRecordCount < pagesize; blankRecordCount++){
								int recordPageIndex = addBlankRecord(&bufferNodes[bufferIndex], pagesize);
								
								if(recordPageIndex < 0)
									return FALSE;
							}*/
							break;
						}
					}
					bufferNodes[bufferIndex].fileNode = currentRunFile;
					buffersInUse++;
					currentRunFile = currentRunFile->nextFileNode;
					if (currentRunFile == NULL)
						break;
				}
				else
					break;
			}
/*set firstFileForRun for next run*/
			firstFileForRun = currentRunFile;

/*while all buffers are not empty (there is still records in pages in
some buffer not including the output buffer)*/
			while(buffersInUse > 0 && rHeap.count > 0){
/*keep getting min record and writing to output buffer*/
/*get smallest record*/
				RecordIndex minIndex = removeMinHeap(&rHeap);
				if(minIndex.guildID == 0)
					return FALSE;
/*move smallest record from main buffer memory to output buffer*/
/*add record to output buffer*/
				addRecord(&bufferNodes[outputBuffIndex],
					minIndex.buff->pageRecords[minIndex.pgIndex], pagesize, bufferNodes[outputBuffIndex].recordCount);
/*remove the same record from original buffer*/
				removeRecord(minIndex.buff, minIndex.pgIndex);
/*if output buffer is full, write page to file*/
				if(bufferNodes[outputBuffIndex].recordCount == pagesize){
/*write page to file*/
					int written;
					written = fwrite(bufferNodes[outputBuffIndex].pageRecords, sizeof(Record),
						pagesize, outputFile);
					if(written !=pagesize){
						fprintf(stderr, "Error: Failed to write to output file, wrote %i records\n",written);
						return FALSE;
					}

					/*clear page in output buffer*/
					clearPage(&bufferNodes[outputBuffIndex], pagesize);
				}

/*if original buffer is empty, read in another page*/
				if(minIndex.buff->recordCount == 0){
					int recordPageIndex;
/*fill buffer with records from file*/
					for(recordPageIndex = 0; recordPageIndex < pagesize; recordPageIndex++){
						Record record;
						if(minIndex.buff->fileNode->fp != NULL){
							if(fread(&record, sizeof(Record), 1, minIndex.buff->fileNode->fp) == 1){
		/*add record to page (records array)*/
								if(addRecord(minIndex.buff, record, pagesize, recordPageIndex) == FALSE)
									return FALSE;
		/*add record index to heap*/
								if(addToHeap(&rHeap, minIndex.buff, recordPageIndex) == FALSE)
									return FALSE;
							}
		/*else reached file end*/
							else{
								/*temp file will be automatically deleted by the system*/
								fclose(minIndex.buff->fileNode->fp);
								minIndex.buff->fileNode->fp = NULL;
								/*removeFile(currentFiles, minIndex.buff->fileNode);*/
								break;
							}
						}
					}
				}
/*if buffer is still empty, then 0 records were read in,
therefore file is empty and the buffer is now free*/
				if(minIndex.buff->recordCount == 0)
					/*decrement buffers in use counter*/
					buffersInUse--;
			}

/*All files for run have been fully read*/
/*Write out records still in output buffer*/
			if(bufferNodes[outputBuffIndex].recordCount > 0){
/*Output buffer page was not full*/
				int i = 0;
				for(i = 0; i < pagesize; i++){
					if(bufferNodes[outputBuffIndex].pageRecords[i].GuildID != 0){
						fwrite(&bufferNodes[outputBuffIndex].pageRecords[i],
							sizeof(Record), 1, outputFile);
						removeRecord(&bufferNodes[outputBuffIndex], i);
					}
				}
			}
			/*Rewind outfile for future merge*/
			rewind(outputFile);
			runCount++;
		}
		/*set runFileListas new current file list*/
		freeFileNode(&currentFiles);
		currentFiles = runFileList;
		*passes = *passes+1;
		*runs = *runs + runCount;
		printf("Pass %i resulted in %i runs\n",*passes,runCount);
	}


/*FileList will contain link to only 1 temporary binary file*/
	if(currentFiles.fileCount != 1){
		fprintf(stderr, "Error: Number of files:%i is invalid\n",currentFiles.fileCount);
		return FALSE;
	}
	*sorted = currentFiles.fileHeadNode->fp;

	/*free allocated memory*/

	for(bufferIndex = 0; bufferIndex < availableBuffers; bufferIndex++){
		freeBuffer(&bufferNodes[bufferIndex]);
	}
	free(bufferNodes);

	freeHeap(&rHeap);

	freeFileNode(&currentFiles);

	/*free(currentFiles);*/

	return TRUE;
}