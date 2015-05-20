/*-------------------------------------------------------------------------------
 * File Organisation, Sorting, and Query Evaluation
 * Name				: Caspersz, Joshua
 * File				: mergesort.c
 * Description		: Mergesort program main
 -------------------------------------------------------------------------------*/
#include "mergesort.h"

int main(int argc, char **argv){
	/*'The number of records that fit in a page'*/
	int pagesize;
	/*'The number of buffer pages that are available in memory for 
		processing purposes'*/
	int buffers;
	/*'The name of the file to be sorted.'*/
	char * path;
	FILE * sortedBinary;

	int passes = 0;
	int runs = 0;
	Schema schema;
	char * fileOutput;
	unsigned long start, end;

	/*Get argument values*/
	if (argc == 4){
		pagesize = strtol(argv[1],NULL,10);
		buffers = strtol(argv[2],NULL,10);
		path = argv[3];
	}
	else{
		fprintf(stderr, "Error: arguments not in the form of:\n"
			"\t./mergesort <pagesize> <buffers> <path>\n");
		return EXIT_FAILURE;
	}

	start = gettime();
	if(externalMergeSort(&sortedBinary, pagesize, buffers, path, &passes, &runs, &schema) == FALSE)
		return EXIT_FAILURE;
	end = gettime() - start;

	printf("Total number of runs  \t%i\n", runs);
	printf("Total number of passes\t%i\n", passes);
	printf("Sorting duration:\t%.6f seconds\n", (double)end/1E6);

	/*save the sorted binary file into a plain text file*/
	if(schema == CHARACTER)
		fileOutput = "characters.sorted";
	else 
		fileOutput = "guilds.sorted";

	if(saveBinaryFileAsText(sortedBinary, fileOutput, schema) == TRUE)
		printf("File %s, successfully sorted and saved as %s\n",path,fileOutput);

	fclose(sortedBinary);

	return EXIT_SUCCESS;
}
