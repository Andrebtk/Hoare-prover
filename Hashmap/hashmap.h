#ifndef HASHMAP_H
#define HASHMAP_H

#include <z3.h>

typedef struct HashEntry_ {
	char *key;
	Z3_ast value;
	struct HashEntry_* next;
} HashEntry;

typedef struct HashMap_ {
	int size;
	HashEntry** table;
} HashMap;


HashMap* create_HashMap(int size);
int hash(HashMap* h,const char* str);
HashEntry* create_HashEntry(const char* key, Z3_ast node);
void insert_HashMap(HashMap* h, const char* name, Z3_ast node);

#endif
