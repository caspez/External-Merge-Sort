/*-------------------------------------------------------------------------------
 * File Organisation, Sorting, and Query Evaluation
 * Name				: Caspersz, Joshua
 * File				: utility.h
 * Description		: Utility functions and definitions
 -------------------------------------------------------------------------------*/

/*System wide header files*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>

#ifndef UTILITY_H_
#define UTILITY_H_

#define LINE_LENGTH 30
#define GUILD_ID_MAX 65535

#pragma pack(push, 1)
/*The record definition of a union of the character and guild schemas */
typedef struct record {
	/*Use either Character struct or guild name char for record*/
	union{
		struct{
			char cname[39];
			unsigned short int team;
			unsigned short int level;
			unsigned int cid;
		}character;
		char g_name[49];
	}field;
	unsigned int GuildID;
}Record;
#pragma pack(pop)

/*/*Set boolean type*/
typedef enum truefalse{
	FALSE=0,TRUE
}BOOLEAN;

/*Set scheme type*/
typedef enum {CHARACTER=0, GUILD=1} Schema;

BOOLEAN saveBinaryFileAsText(FILE * binary, char * fileName, Schema schema);
unsigned long gettime();

#endif