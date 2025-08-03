#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data-struct.h"



ASTNode* create_node_binary(char* type, ASTNode* left, ASTNode* right) {
    ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_BIN_OP;

    res->binary_op.op = strdup(type);
    res->binary_op.left = left;
    res->binary_op.right = right;
    return res;
}

ASTNode* create_node_number(int num) {
    ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_NUMBER;

    res->number = num;
    return res;
}

ASTNode* create_node_id(char *input) {
    ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_ID;

    res->id_name = strdup(input);
    return res;
}


ASTNode* create_node_assign(char* id, ASTNode* expr){
    ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_ASSIGN;
    res->Assign.id = strdup(id);
    res->Assign.expr = expr;
    return res;
}

ASTNode* create_node_If(ASTNode* condition, DLL* block) {
    ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_IF;

    res->If_While.condition = condition;
    res->If_While.block = block;
    return res;
}

ASTNode* create_node_While(ASTNode* condition, DLL* block){
     ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_WHILE;

    res->If_While.condition = condition;
    res->If_While.block = block;
    return res;
}



DLL* create_DLL() {
    DLL* res = malloc(sizeof(DLL));
    res->first = NULL;
    res->last = NULL;
    return res;
}

line_linkedlist* create_ll(ASTNode* node) {
    line_linkedlist* list = malloc(sizeof(line_linkedlist));
    list->next = NULL;
    list->node=node;
    return list;
}

void DLL_append(DLL* list, ASTNode* node) {
    line_linkedlist* l = create_ll(node);
    
    if(list->last == NULL) {    //0 element in list
        list->first = l;
        list->last = l;
    } else {
        list->last->next = l;
        list->last = l;
    }
}

void print_line(int iter) {
	if(iter != -1) {
		if(iter == 1) {
			printf("1st instruction: \n");
		}else {
			printf("%dnd instruction: \n", iter);
		}
	}
}

void print_prof(int prof) {
	for (int i=0; i<prof; i++) {
		printf("\t");
	}
}



void print_ASTNode(ASTNode* node, int iter, int prof) {
    if(node == NULL) {
        printf("node is NULL");
        return;
    }
    
    switch(node->type) {
        case NODE_BIN_OP: {
			print_line(iter);
			print_prof(prof);
            printf("Node binary: %s\n", node->binary_op.op);
			
			print_prof(prof);
            printf("Left node: \n");
            print_ASTNode(node->binary_op.left, -1, prof+1);
            
			print_prof(prof);
			printf("Right node: \n");
            print_ASTNode(node->binary_op.right, -1, prof+1);
            break;
        };

        case NODE_ID: {
			print_line(iter);
			print_prof(prof);
            printf("Node ID: \n");
			print_prof(prof+1);
			printf("%s\n",node->id_name);

            break;
        };

        case NODE_NUMBER: {
			print_line(iter);
			print_prof(prof);
            printf("Node NUMBER: \n");

			print_prof(prof+1);
			printf("%d", node->number);
			printf("\n");
            break;
        };

        case NODE_ASSIGN: {
			print_prof(prof-1);
			print_line(iter);

			print_prof(prof);
            printf("Node ASSIGN: \n");

			print_prof(prof);
			printf("left: %s\n",node->Assign.id);
            
			print_prof(prof);
			printf("Expr: \n");
            print_ASTNode(node->Assign.expr, -1, prof+1);
            printf("\n");

            break;
        };

        case NODE_IF: {
			print_line(iter);
			print_prof(prof);
            printf("Node IF:\n");
			
			print_prof(prof);
            printf("condition : \n");
            print_ASTNode(node->If_While.condition, -1, prof+1);
			
			printf("\n");
			print_prof(prof);
            printf("block : \n");

            //print_prof(prof+1);
			print_DLL(node->If_While.block, prof+1);
			printf("\n");
            break;
        };

        case NODE_WHILE: {
			print_prof(prof-1);
			print_line(iter);
			print_prof(prof);
          	printf("Node WHILE:\n");
			
			print_prof(prof);
            printf("condition : \n");
            print_ASTNode(node->If_While.condition, -1, prof+1);


			printf("\n");
			print_prof(prof);
            printf("block : \n");
            print_DLL(node->If_While.block, prof+1);
			printf("\n");
            break;
        };

        case NODE_BLOCK: {
			print_prof(prof);
			print_line(iter);
            printf("Node block");
			printf("\n");
            break;
        };

        
        default:
			print_line(iter);
            printf("Unknown node type: %d\n", node->type);
			printf("\n");
            break;
    }
    
    
}

void print_line_linkedlist(line_linkedlist* list, int prof) {
	int num = 1;
	int p = 1 + prof;

    while(list != NULL) {
        if (list->node == NULL) {
            printf("Null node in linked list\n");
        } else {
           print_ASTNode(list->node, num, p);
        }
        list = list->next;
		num++;
	}
}

void print_DLL(DLL* dll, int prof) {
    if(dll == NULL) {
        printf("DLL is NULL\n");
        return;
    }

    if(dll->first == NULL ){
        printf("DLL first is NULL\n");
    }
    print_line_linkedlist(dll->first, prof);
    
    printf("PRECONDITON: \n");
    print_ASTNode(dll->pre, 1, prof+1);

    printf("POSTCONDITON: \n");
    print_ASTNode(dll->post, 1, prof+1);
   
}


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

HashEntry* create_HashEntry(const char* key, ASTNode* node) {
    HashEntry* res = malloc(sizeof(HashEntry));
    res->key = strdup(key);
    res->value = node;
    res->next = NULL;
    return res;
}


void insert_HashMap(HashMap* h, const char* name, ASTNode* node) {
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
