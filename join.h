/*-------------------------------------------------------------------------------
 * File Organisation, Sorting, and Query Evaluation
 * Name				: Caspersz, Joshua
 * File				: join.h
 * Description		: Sort-Merge Join functions
 -------------------------------------------------------------------------------*/

#ifndef JOIN_H
#define JOIN_H

#include "utility.h"

BOOLEAN sortMergeJoin(int pagesize, int buffers, FILE * sortedFileA, FILE * sortedFileB, Schema aSchema, BOOLEAN output, unsigned * tuples);

#endif