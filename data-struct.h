#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include <stdio.h>

typedef enum { NODE_ASSIGN, NODE_BIN_OP, NODE_IF, NODE_WHILE, NODE_NUMBER, NODE_ID, NODE_BLOCK} NodeType;
typedef enum { num, id, plus, minus, mul, div2 } TypeExpr;
typedef enum { lt, gt, eq, neq } TypeCondition;

struct DLL_;
typedef struct DLL_ DLL;

typedef struct ASTNode_ {
	NodeType type;
	
	union {
		struct {
			char* id;
			struct ASTNode_* expr;
		} Assign;

		
		struct {
			struct ASTNode_* condition;
			DLL* block;
		} If_While;

		struct {
			char *op;
			struct ASTNode_* left;
			struct ASTNode_* right;
		} binary_op;

		int number;
		char* id_name;
	};
	

} ASTNode;

typedef struct line_linkedlist_ {
	ASTNode *node;
	struct line_linkedlist_* next;
} line_linkedlist;

typedef struct DLL_ {
	line_linkedlist* first;
	line_linkedlist* last;
	ASTNode *pre;
	ASTNode *post;
} DLL;



// HashMap
typedef struct HashEntry_ {
	char *key;
	ASTNode* value;
	struct HashEntry_* next;
} HashEntry;

typedef struct HashMap_ {
	int size;
	HashEntry** table;
} HashMap;





ASTNode* create_node_binary(char* type, ASTNode* left, ASTNode* right);
ASTNode* create_node_number(int num);
ASTNode* create_node_id(char *input);
ASTNode* create_node_assign(char* id, ASTNode* expr);
ASTNode* create_node_If(ASTNode* condition, DLL* block);
ASTNode* create_node_While(ASTNode* condition, DLL* block);

DLL* create_DLL();
line_linkedlist* create_ll(ASTNode* node);
void DLL_append(DLL* list, ASTNode* node);

void print_ASTNode(ASTNode* node, int iter, int prof);
void print_line_linkedlist(line_linkedlist* list, int prof);
void print_DLL(DLL* dll, int prof);


HashMap* create_HashMap(int size);
int hash(HashMap* h,const char* str);

#endif