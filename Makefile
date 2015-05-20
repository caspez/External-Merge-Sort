##################################################################################
 # File Organisation, Sorting, and Query Evaluation
 # Name			: Caspersz, Joshua
 # File			: Makefile
 # Description		: Makefile of program
##################################################################################

#Define compiler flags
FLAGS = -ansi -Wall -pedantic
LIB = -lm
#Define common source files
SRC = utility.c fileList.c sort.c recordHeap.c buffer.c

.PHONY : all
all: m s

m:
	gcc $(FLAGS) $(SRC) mergesort.c -o mergesort $(LIB)

s:
	gcc $(FLAGS) $(SRC) sortmerge.c join.c -o sortmerge $(LIB)

clean:
	rm -rf *.o *.zip core *.sorted *.errs .*.*.swp sortmerge mergesort

clearscreen:
	clear

.PHONY: c
c: clearscreen clean all