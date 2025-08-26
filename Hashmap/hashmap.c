#include "hashmap.h"
#include <stdlib.h>
#include <string.h>

HashMap* create_HashMap(int size) {
    HashMap* res = malloc(sizeof(HashMap));
    res->size = size;
    res->table = malloc(sizeof(HashEntry*) * size);
    
    for(int i=0; i<size; i++) {
        res->table[i] = NULL;
    }
    
    return res;
} 

unsigned long hash_djb2(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; // hash * 33 + c

    return hash;
}

int hash(HashMap* h,const char* str) {
    return (int) (hash_djb2(str) % h->size);
}

HashEntry* create_HashEntry(const char* key, Z3_ast node) {
    HashEntry* res = malloc(sizeof(HashEntry));
    res->key = strdup(key);
    res->value = node;
    res->next = NULL;
    return res;
}

void insert_HashMap(HashMap* h, const char* name, Z3_ast node) {
    int index = hash(h, name);
    HashEntry* n = create_HashEntry(name, node);

    HashEntry* tmp = h->table[index];

    if(tmp == NULL) {
        h->table[index] = n;
        return;
    }

    HashEntry* prev = NULL;
    while (tmp != NULL) {
        if (strcmp(tmp->key, name) == 0) {
            tmp->value = node;
            return;
        }
        prev = tmp;
        tmp = tmp->next;
    }

    prev->next = n;
    
}



void free_hashmap(HashMap* hm) {
	if (!hm) return;
	for (int i = 0; i < hm->size; ++i) {
		HashEntry* cur = hm->table[i];
		while (cur) {
			HashEntry* next = cur->next;
			if (cur->key) free(cur->key);
			/* Do NOT free cur->value (Z3_ast) here — no context to dec_ref safely */
			free(cur);
			cur = next;
		}
	}
	free(hm->table);
	free(hm);
}

void free_hashmap_with_context(HashMap* map, Z3_context ctx) {
    if (!map) return;
    
    for (int i = 0; i < map->size; i++) {
        HashEntry* entry = map->table[i];
        while (entry) {
            HashEntry* next = entry->next;
            
            // Decrement Z3 reference for the value
            if (entry->value && ctx) {
                Z3_dec_ref(ctx, (Z3_ast)entry->value);
            }
            
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(map->table);
    free(map);
}