/*-------------------------------------------------------------------------------
 * File Organisation, Sorting, and Query Evaluation
 * Name				: Caspersz, Joshua
 * File				: sortmerge.h
 * Description		: Sortmerge program main
 -------------------------------------------------------------------------------*/
#include "sortmerge.h"

int main(int argc, char **argv){
	/*'The number of records that fit in a page'*/
	int pagesize;
	/*'The number of buffer pages that are available in memory for 
		processing purposes'*/
	int buffers;
	/*'The name of the file to be sorted.'*/
	char * path1;
	char * path2;
	FILE * sortedFile1;
	FILE * sortedFile2;

	int passes = 0;
	int runs = 0;
	Schema schemaA;
	Schema schemaB;
	unsigned tuples = 0;
	unsigned long start, end;
	int argI = 1;
	BOOLEAN output = FALSE;

	/*Get argument values*/
	if (argc == 6){
		char * option = argv[argI++];
		if (strcmp(option,"-o")!=0)
			fprintf(stderr, "Invalid argument: %s\n",option);
		else
			output = TRUE;
	}
	if (argc == 5 || (argc == 6 && output == TRUE)){
		pagesize = strtol(argv[argI++],NULL,10);
		buffers = strtol(argv[argI++],NULL,10);
		path1 = argv[argI++];
		path2 = argv[argI++];
	}
	else{
		fprintf(stderr, "Error: arguments not in the form of either:\n"
			"\t./sortmerge [-o] <pagesize> <buffers> <path>/characters <path>/guilds\n"
			"\tor\n"
			"\t./sortmerge [-o] <pagesize> <buffers> <path>/guilds <path>/characters\n");
		return EXIT_FAILURE;
	}

	start = gettime();
	if(externalMergeSort(&sortedFile1, pagesize, buffers, path1, &passes, &runs, &schemaA) == FALSE)
		return EXIT_FAILURE;
	if(externalMergeSort(&sortedFile2, pagesize, buffers, path2, &passes, &runs, &schemaB) == FALSE)
		return EXIT_FAILURE;
	if(sortMergeJoin(pagesize, buffers, sortedFile1, sortedFile2, schemaA, output, &tuples) == FALSE)
		return EXIT_FAILURE;
	end = gettime() - start;

	fclose(sortedFile1);
	fclose(sortedFile2);

	printf("Number of tuples: %i\n", tuples);
	printf("Time: %.6f seconds\n", (double)end/1E6);

	return EXIT_SUCCESS;
}
