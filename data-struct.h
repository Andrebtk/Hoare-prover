#include <stdio.h>

typedef enum { NODE_ASSIGN, NODE_BIN_OP, NODE_IF, NODE_WHILE, NODE_NUMBER, NODE_ID, NODE_BLOCK} NodeType;
typedef enum { num, id, plus, minus, mul, div } TypeExpr;
typedef enum { lt, gt, eq, neq } TypeCondition;

typedef struct ASTNode_ {
	NodeType type;
	
	union {
		struct {
			char* id;
			struct ASTNode_* expr;
		} Assign;

		
		struct {
			struct ASTNode_* condition;
			struct DLL* block;
		} If_While;

		struct {
			char *op;
			struct ASTNode_* left;
			struct ASTNode_* right;
		} binary_op;
	};
	

} ASTNode;

typedef struct line_linkedlist_ {
	ASTNode *node;
	struct line_linkedlist_* next;
} line_linkedlist;

typedef struct DLL_ {
	line_linkedlist* first;
	line_linkedlist* last;
} DLL;



ASTNode* create_node_assign();
ASTNode* create_node_Expression();

/*
data structure for each type of statement 
1st type of node -> identifier/NUMbers etc
2nd type of node -> expr/ assign/ if/ while
*/