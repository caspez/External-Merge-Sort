/*-------------------------------------------------------------------------------
 * File Organisation, Sorting, and Query Evaluation
 * Name				: Caspersz, Joshua
 * File				: utility.c
 * Description		: Utility functions
 -------------------------------------------------------------------------------*/
#include "utility.h"

BOOLEAN saveBinaryFileAsText(FILE * binary, char * fileName, Schema schema){
	FILE * outputFile;
	Record record;

	if(binary == NULL){
		fprintf(stderr, "Error: Unable to read binary file\n");
		return FALSE;
	}

	outputFile = fopen(fileName,"w");
	/*rewind(binary);*/

	if(schema == CHARACTER){
		while(fread(&record, sizeof(Record), 1, binary) == 1){	
			fprintf(outputFile,"%s,%i,%i,%i,%i\n",record.field.character.cname, 
				record.field.character.team, record.field.character.level,
				record.field.character.cid, record.GuildID);
		}
	}
	else{
		while(fread(&record, sizeof(Record), 1, binary) == 1){
			fprintf(outputFile,"%i,%s\n",record.GuildID, record.field.g_name);
		}
	}
	fclose(outputFile);
	return TRUE;
}

/*get time function sourced from Algorithms and Analysis 2014 Summer Semester*/
unsigned long gettime(){
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return (tp.tv_sec * 1E6) + tp.tv_usec;
}