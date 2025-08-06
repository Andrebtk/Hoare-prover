#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include <stdio.h>
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define RESET   "\033[0m"


typedef enum { NODE_ASSIGN, NODE_BIN_OP, NODE_IF_ELSE, NODE_WHILE, NODE_NUMBER, NODE_ID, NODE_BLOCK, NODE_FUNCTION} NodeType;
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
			char* fname;
			struct ASTNode_* arg1;
			struct ASTNode_* arg2;
		} function;
		
		struct {
			struct ASTNode_* condition;
			DLL* block_main;
			DLL* block_else;
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
	struct line_linkedlist_* prec;
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
ASTNode* create_node_If_Else(ASTNode* condition, DLL* block_if, DLL* block_else);
ASTNode* create_node_While(ASTNode* condition, DLL* block);
ASTNode* create_node_Func(const char* name, ASTNode* a1, ASTNode* a2);

DLL* create_DLL();
line_linkedlist* create_ll(ASTNode* node);
void DLL_append(DLL* list, ASTNode* node);

void print_ASTNode(ASTNode* node, int iter, int prof);
void print_line_linkedlist(line_linkedlist* list, int prof);
void print_DLL(DLL* dll, int prof, int pre);


HashMap* create_HashMap(int size);
int hash(HashMap* h,const char* str);
void insert_HashMap(HashMap* h, const char* name, ASTNode* node);


ASTNode* substitute(ASTNode* formula, const char* var, ASTNode* replacement);
ASTNode* clone_node(const ASTNode* orig);
void hoare_prover(DLL* code);
int evaluate_formula(ASTNode* node);
int evaluate_expr (ASTNode* node);


#endif