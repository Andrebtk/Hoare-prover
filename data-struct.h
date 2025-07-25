#include <stdio.h>


typedef struct linkedList_ {
	struct linkedList_* next;

} linkedList;

typedef struct doubleLL_ {
	linkedList* first;
	linkedList* last;
} doubleLL;


/*
data structure for each type of statement 
1st type of node -> identifier/NUMbers etc
2nd type of node -> expr/ assign/ if/ while


*/