#ifndef AST_H
#define AST_H

#include <z3.h>


typedef enum { NODE_ASSIGN, NODE_BIN_OP, NODE_IF_ELSE, NODE_WHILE, NODE_NUMBER, 
					NODE_ID, NODE_FUNCTION, NODE_UNARY_OP, NODE_BOOL} NodeType;


struct DLL_;
typedef struct DLL_ DLL;

typedef struct ASTNode_ {
	NodeType type;
	int freed;

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
			DLL* block_if;
			DLL* block_else;
		} If;

		struct {
			struct ASTNode_* condition;
			struct ASTNode_* invariant;
			struct ASTNode_* variant;
			DLL* block_main;
		} While;

		struct {
			char *op;
			struct ASTNode_* left;
			struct ASTNode_* right;
		} binary_op;

		struct {
			char *op;
			struct ASTNode_*child;
		} unary_op;

		int number;
		char* id_name;
		int bool_value;
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



ASTNode* create_node_binary(char* type, ASTNode* left, ASTNode* right);
ASTNode* create_node_unary(char* type, ASTNode* child);
ASTNode* create_node_number(int num);
ASTNode* create_node_id(char *input);
ASTNode* create_node_assign(char* id, ASTNode* expr);
ASTNode* create_node_If_Else(ASTNode* condition, DLL* block_if, DLL* block_else);
ASTNode* create_node_While(ASTNode* condition, DLL* block, ASTNode* invariant, ASTNode* variant);
ASTNode* create_node_Func(const char* name, ASTNode* a1, ASTNode* a2);
ASTNode* create_node_bool(int value);

DLL* create_DLL();
line_linkedlist* create_ll(ASTNode* node);
void DLL_append(DLL* list, ASTNode* node);

void print_ASTNode(ASTNode* node, int iter, int prof);
void print_line_linkedlist(line_linkedlist* list, int prof);
void print_DLL(DLL* dll, int prof, int pre);

ASTNode* substitute(const ASTNode* node, const char* id, const ASTNode* repl);
ASTNode* clone_node(const ASTNode* orig);

void free_ll(line_linkedlist* l);
void free_DLL(DLL* l);
void free_ASTNode(ASTNode* node);

#endif