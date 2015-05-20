/*-------------------------------------------------------------------------------
* File Organisation, Sorting, and Query Evaluation
* Name				: Caspersz, Joshua
* File				: join.c
* Description		: Sort-Merge Join functions
* Based on this algorithm: http://www.dcs.ed.ac.uk/home/tz/phd/thesis/node20.htm
-------------------------------------------------------------------------------*/
#include "join.h"

void printRelation(Record aTuple, Record bTuple, Schema aSchema){
	printf("%i,",aTuple.GuildID);
	if(aSchema == CHARACTER)
		printf("%s,%i,%i,%i,%s\n",aTuple.field.character.cname, 
				aTuple.field.character.team, aTuple.field.character.level,
				aTuple.field.character.cid,bTuple.field.g_name);
	else
		printf("%s,%s,%i,%i,%i\n",aTuple.field.g_name,bTuple.field.character.cname, 
				bTuple.field.character.team, bTuple.field.character.level,
				bTuple.field.character.cid);
}

/*Method assumes both files have been sorted and file pointers point to the start of the file*/
BOOLEAN sortMergeJoin(int pagesize, int buffers, FILE * sortedFileA, FILE * sortedFileB, Schema aSchema, BOOLEAN output, unsigned * tuples){
	Record * aTuples, * bTuples;
	int aTIndex = 0, bTIndex = 0;
	int availableMemory = pagesize*buffers;
	int aBufferMemoryCount = availableMemory*0.5;
	int bBufferMemoryCount = availableMemory*0.5;
	int comparisonCount = 0;

	/*if evenly divisible of buffers*/
	if(availableMemory-aBufferMemoryCount+bBufferMemoryCount == 0){
		/*Decrease 1 tuple from both, since an extra tuple space is needed in memory
		when checking fir duplicate tuples*/
		aBufferMemoryCount--;
		bBufferMemoryCount--;
	}


	aTuples = calloc(aBufferMemoryCount, sizeof(Record));
	if (!aTuples){
		fprintf(stderr, "Error: Failed to allocate memory for aTuples\n");
		return FALSE;
	}
	bTuples = calloc(bBufferMemoryCount, sizeof(Record));
	if (!bTuples){
		fprintf(stderr, "Error: Failed to allocate memory for bTuples\n");
		return FALSE;
	}

	fread(aTuples, sizeof(Record), aBufferMemoryCount, sortedFileA);
	fread(bTuples, sizeof(Record), bBufferMemoryCount, sortedFileB);

	/*Get first tuples from both files*/
	while(aBufferMemoryCount > 0 && bBufferMemoryCount > 0){
		if(aTuples[aTIndex].GuildID > bTuples[bTIndex].GuildID){
			comparisonCount++;
			/*get next tuple from b by increasing index*/
			bTIndex++;
			if(bBufferMemoryCount <= bTIndex){
				/*if read through all tuples in the buffer*/
				bBufferMemoryCount = fread(bTuples, sizeof(Record), bBufferMemoryCount, sortedFileB);
				bTIndex = 0;
			}
		}
		else if(aTuples[aTIndex].GuildID < bTuples[bTIndex].GuildID){
			comparisonCount += 2;
			/*get next tuple from a by increasing index*/
			aTIndex++;
			if(aBufferMemoryCount <= aTIndex){
				/*if read through all tuples in the buffer*/
				aBufferMemoryCount = fread(aTuples, sizeof(Record), aBufferMemoryCount, sortedFileA);
				aTIndex = 0;
			}
		}
		else{/*guild ids are equal*/
			fpos_t aFilePos;
			fpos_t bFilePos;
			Record duplicateRecord;
			int aDupTIndex = aTIndex + 1;
			int bDupTIndex = bTIndex + 1;
			BOOLEAN fileEnd = FALSE;
			
			comparisonCount += 2;

			if (fgetpos(sortedFileA, &aFilePos) != 0){
				fprintf(stderr, "Error: Failed to get sortedFileA position\n");
				return FALSE;
			}
			if (fgetpos(sortedFileB, &bFilePos) != 0){
				fprintf(stderr, "Error: Failed to get sortedFileA position\n");
				return FALSE;
			}

			if(output == TRUE)
				printRelation(aTuples[aTIndex], bTuples[bTIndex], aSchema);
			*tuples = *tuples + 1;


			/*print other b tuples that match with a(duplicates)*/
			/*While there is tuples to read from array and then file
			 and the read in tuples equals TupleA's GuildID*/
			if(bBufferMemoryCount <= bDupTIndex){
				if(fread(&duplicateRecord, sizeof(Record), 1, sortedFileB) != 1)
					fileEnd = TRUE;
			}
			else
				duplicateRecord = bTuples[bDupTIndex];

			comparisonCount++;
			while(fileEnd == FALSE && (aTuples[aTIndex].GuildID == duplicateRecord.GuildID)){
				comparisonCount++;
				if(output == TRUE)
					printRelation(aTuples[aTIndex], duplicateRecord, aSchema);
				*tuples = *tuples + 1;

				if(bBufferMemoryCount <= ++bDupTIndex){
					if(fread(&duplicateRecord, sizeof(Record), 1, sortedFileB) != 1)
						fileEnd = TRUE;
				}
				else
					duplicateRecord = bTuples[bDupTIndex];
			}

			fileEnd = FALSE;
			/*print other b tuples that match with a(duplicates)*/
			/*While there is tuples to read from array and then file
			 and the read in tuples equals TupleA's GuildID*/
			if(aBufferMemoryCount <= aDupTIndex){
				if(fread(&duplicateRecord, sizeof(Record), 1, sortedFileA) != 1)
					fileEnd = TRUE;
			}
			else
				duplicateRecord = aTuples[aDupTIndex];

			comparisonCount++;
			while(fileEnd == FALSE && (duplicateRecord.GuildID == bTuples[bTIndex].GuildID)){
				comparisonCount++;
				if(output == TRUE)
					printRelation(duplicateRecord, bTuples[bTIndex], aSchema);
				*tuples = *tuples + 1;

				if(aBufferMemoryCount <= ++aDupTIndex){
					if(fread(&duplicateRecord, sizeof(Record), 1, sortedFileA) != 1)
						fileEnd = TRUE;
				}
				else
					duplicateRecord = aTuples[aDupTIndex];
			}


			/*set file pointers back to previous location*/
			if (fsetpos(sortedFileA, &aFilePos) != 0){
				fprintf(stderr, "Error: Failed to set sortedFileA position\n");
				return FALSE;
			}
			if (fsetpos(sortedFileB, &bFilePos) != 0){
				fprintf(stderr, "Error: Failed to set sortedFileA position\n");
				return FALSE;
			}

			/*increase tuple index of both files*/
			if(++aTIndex >= aBufferMemoryCount){
				aBufferMemoryCount = fread(aTuples, sizeof(Record), aBufferMemoryCount, sortedFileA);
				aTIndex = 0;
			}
			if(++bTIndex >= bBufferMemoryCount){
				bBufferMemoryCount = fread(bTuples, sizeof(Record), bBufferMemoryCount, sortedFileB);
				bTIndex = 0;
			}
		}
	}

	free(aTuples);
	free(bTuples);

	printf("Comparison Count: %i:\n",comparisonCount);

	return TRUE;
}
