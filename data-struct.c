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

ASTNode* create_node_unary(char* type, ASTNode* child) {
	ASTNode* res = malloc(sizeof(ASTNode));
	res->type = NODE_UNARY_OP;

	res->unary_op.op = strdup(type);
	res->unary_op.child = child;

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

ASTNode* create_node_If_Else(ASTNode* condition, DLL* block_if, DLL* block_else) {
    ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_IF_ELSE;

    res->If_While.condition = condition;
    res->If_While.block_main = block_if;
	res->If_While.block_else = block_else;
    return res;
}

ASTNode* create_node_While(ASTNode* condition, DLL* block){
    ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_WHILE;

    res->If_While.condition = condition;
    res->If_While.block_main = block;
    return res;
}

ASTNode* create_node_Func(const char* name, ASTNode* a1, ASTNode* a2) {
	ASTNode* res = malloc(sizeof(ASTNode));
	res->type = NODE_FUNCTION;

	res->function.fname = strdup(name);
	res->function.arg1 = a1;
	res->function.arg2 = a2;
	return res;
}

ASTNode* create_node_bool(int value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_BOOL;
    node->bool_value = value;
    return node;
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
	list->prec = NULL;
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
		l->prec = list->last;
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
			if (node->binary_op.left)
				print_ASTNode(node->binary_op.left, -1, prof+1);
			else
				printf("NULL\n");

			print_prof(prof);
			printf("Right node: \n");
			if (node->binary_op.right)
				print_ASTNode(node->binary_op.right, -1, prof+1);
			else
				printf("NULL\n");

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
			printf("left: %s\n", node->Assign.id);

			print_prof(prof);
			printf("Expr: \n");

			if (node->Assign.expr)
				print_ASTNode(node->Assign.expr, -1, prof+1);
			else
				printf("NULL\n");

			printf("\n");
			break;
		}

        case NODE_IF_ELSE: {
			print_line(iter);
			print_prof(prof);
            printf("Node IF ELSE:\n");
			
			print_prof(prof);
            printf("condition : \n");
            print_ASTNode(node->If_While.condition, -1, prof+1);
			
			printf("\n");
			print_prof(prof);
            printf("block IF : \n");

            //print_prof(prof+1);
			print_DLL(node->If_While.block_main, prof+1, -1);
			printf("\n");

			print_prof(prof);
            printf("block Else : \n");

            //print_prof(prof+1);
			print_DLL(node->If_While.block_else, prof+1, -1);
			printf("\n");
            break;
        };

		case NODE_FUNCTION: {
			print_line(iter);
			print_prof(prof);
			printf("Node Function:\n");

			print_prof(prof);
			printf("Function name: %s\n", node->function.fname);

			print_prof(prof);
			printf("Expr arg1: \n");
			if (node->function.arg1)
				print_ASTNode(node->function.arg1, -1, prof+1);
			else
				printf("NULL\n");

			print_prof(prof);
			printf("Expr arg2: \n");
			if (node->function.arg2)
				print_ASTNode(node->function.arg2, -1, prof+1);
			else
				printf("NULL\n");

			break;
		}



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
            print_DLL(node->If_While.block_main, prof+1, -1);
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

void print_DLL(DLL* dll, int prof, int pre) {
    if(dll == NULL) {
        printf("DLL is NULL\n");
        return;
    }

    if(dll->first == NULL ){
        printf("DLL first is NULL\n");
    }
    print_line_linkedlist(dll->first, prof);
    

	if(pre == 0) {
		    printf("PRECONDITON: \n");
		print_ASTNode(dll->pre, 1, prof+1);

		printf("POSTCONDITON: \n");
		print_ASTNode(dll->post, 1, prof+1);
	}

   
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


ASTNode* clone_node(const ASTNode* orig) {
	if (!orig) return NULL;
	switch (orig->type) {

		case NODE_ASSIGN: {
			
			ASTNode* expr = clone_node(orig->Assign.expr);
			return create_node_assign(orig->Assign.id, expr);
			break;
		}

		case NODE_BIN_OP: {

			ASTNode* left = clone_node(orig->binary_op.left);
			ASTNode* right = clone_node(orig->binary_op.right);

			return create_node_binary(orig->binary_op.op, left, right);
			break;
		}

		case NODE_UNARY_OP: { 
			return create_node_unary(
				strdup(orig->unary_op.op),
				clone_node(orig->unary_op.child)
			);
			break;
		}

		case NODE_NUMBER: {
			return create_node_number(orig->number);

			break;
		}

		case NODE_ID: {
			return create_node_id(orig->id_name);

			break;
		}

		case NODE_FUNCTION: {
			return create_node_Func(
				strdup(orig->function.fname),
				clone_node(orig->function.arg1),
				clone_node(orig->function.arg2)
			);

			break;
		}

		case NODE_IF_ELSE: {
			// Clone the “then” block
			DLL* then_copy = create_DLL();
			for (line_linkedlist* it = orig->If_While.block_main->first; it; it = it->next) {
				DLL_append(then_copy, clone_node(it->node));
			}
			// Clone the “else” block
			DLL* else_copy = create_DLL();
			for (line_linkedlist* it = orig->If_While.block_else->first; it; it = it->next) {
				DLL_append(else_copy, clone_node(it->node));
			}
			return create_node_If_Else(
				clone_node(orig->If_While.condition),
				then_copy,
				else_copy
			);

			break;
		}

		case NODE_WHILE: {
			DLL* body_copy = create_DLL();
			for (line_linkedlist* it = orig->If_While.block_main->first; it; it = it->next) {
				DLL_append(body_copy, clone_node(it->node));
			}
			return create_node_While(
				clone_node(orig->If_While.condition),
				body_copy
			);

			break;
		}

		case NODE_BLOCK: {

			DLL* blk = create_DLL();
			for (line_linkedlist* it = orig->If_While.block_main->first; it; it = it->next) {
				DLL_append(blk, clone_node(it->node));
			}
			ASTNode* node = malloc(sizeof(ASTNode));
			node->type = NODE_BLOCK;
			node->If_While.block_main = blk;
			return node;

			break;
		}
	}
}


ASTNode* substitute(ASTNode* formula, const char* var, ASTNode* replacement) {
	if (formula == NULL) return NULL;

	switch (formula->type) {
		case NODE_ASSIGN: {

			ASTNode* new_expr = substitute(formula->Assign.expr, var, replacement);
			return create_node_assign(formula->Assign.id, new_expr);
			break;
		}


		case NODE_BIN_OP: {

			ASTNode* left = substitute(formula->binary_op.left, var, replacement);
			ASTNode* right = substitute(formula->binary_op.right, var, replacement);
			return create_node_binary(formula->binary_op.op, left, right);

			break;
		}


		case NODE_IF_ELSE: {
			return NULL;
			break;
		}

		case NODE_WHILE: {
			return NULL;
			break;
		}


		case NODE_NUMBER: {
			return create_node_number(formula->number);
			break;
		}

		case NODE_ID: {
			if(strcmp(formula->id_name, var) == 0) {
				return clone_node(replacement);
			}
			else {
				return create_node_id(formula->id_name);
			}
			break;
		}

		case NODE_BLOCK: {


		}

		case NODE_FUNCTION: {
			// Substitute in the function's arguments
			ASTNode* new_arg1 = substitute(formula->function.arg1, var, replacement);
			ASTNode* new_arg2 = substitute(formula->function.arg2, var, replacement);
			return create_node_Func(formula->function.fname, new_arg1, new_arg2);
			break;
		}

		case NODE_UNARY_OP: {
			// Substitute in the child subtree
			ASTNode* new_child = substitute(formula->unary_op.child, var, replacement);
			return create_node_unary(formula->unary_op.op, new_child);
			break;
		}

	}
}



ASTNode* hoare_prover(DLL* code, ASTNode* pre, ASTNode* post) {

	if (!code || !post) {
		fprintf(stderr, RED "NULL input to hoare_prover\n" RESET);
		return NULL;
	}

	line_linkedlist *current = code->last;
	ASTNode* wp = post;

	while( current != NULL ) {
		ASTNode* wp_clone = clone_node(wp);
		ASTNode* new_wp   = hoare_statement(current->node, wp_clone);
		// optionally free(wp_clone) if you don't need the old tree any more
		wp = new_wp;
		current = current->prec;
	}

	return wp;
}



ASTNode* hoare_statement(ASTNode* node, ASTNode* post) {
	switch (node->type) {
		case NODE_ASSIGN : {
			return hoare_AssignmentRule(node, post);
			break;
		}

		case NODE_IF_ELSE: {
			return hoare_IfElseRule(node, post);
			break;
		}

		default:
			fprintf(stderr, "hoare_statement: unsupported node type %d\n", node->type);
			return NULL;

	}

}



/*
   {P} if B then { C1 } else { C2 } {Q}
*/
ASTNode* hoare_IfElseRule(ASTNode* node_IfElse, ASTNode* post) {

	if (!node_IfElse || node_IfElse->type != NODE_IF_ELSE) {
		fprintf(stderr, "Invalid node in hoare_IfElseRule\n");
		return NULL;
	}

	
	ASTNode* condition = node_IfElse->If_While.condition;	// B
	DLL* block_if = node_IfElse->If_While.block_main;		// C1
	DLL* block_else = node_IfElse->If_While.block_else;		// C2

	if (!condition || !block_if || !block_else) {
		fprintf(stderr, RED "Incomplete IF node\n" RESET);
		return NULL;
	}


	ASTNode* wp_if = hoare_prover(block_if, NULL, clone_node(post));
	ASTNode* wp_else = hoare_prover(block_else, NULL, clone_node(post));

	ASTNode* left = create_node_binary("->", condition, wp_if);
	ASTNode* n_right = create_node_unary("not", condition);
	ASTNode* right = create_node_binary("->", n_right, wp_else);

	return create_node_binary("and", left, right);
}


ASTNode* hoare_AssignmentRule(ASTNode* node, ASTNode* post /*DLL* code*/) {
	return substitute(post, 
						node->Assign.id, 
						node->Assign.expr);
}


// 2) Logical & comparison evaluator
int evaluate_formula(ASTNode* node) {

	if (!node) return 0;

	switch (node->type) {
		case NODE_BIN_OP : {

			if( strcmp(node->binary_op.op, "AND")==0 || strcmp(node->binary_op.op, "and")==0 ) 
				return evaluate_formula(node->binary_op.left) 
						&& evaluate_formula(node->binary_op.right);

			if( strcmp(node->binary_op.op, "OR")==0 || strcmp(node->binary_op.op, "or")==0 ) 
				return evaluate_formula(node->binary_op.left) 
						|| evaluate_formula(node->binary_op.right);
			
			if( strcmp(node->binary_op.op, "->")==0 ) {
				int L = evaluate_formula(node->binary_op.left);
				int R = evaluate_formula(node->binary_op.right);
				return !L || R;
			}
				

			int L = evaluate_expr(node->binary_op.left);
			int R = evaluate_expr(node->binary_op.right);

			if( strcmp(node->binary_op.op, "==")==0 ) return L == R;
			if( strcmp(node->binary_op.op, "!=")==0 ) return L != R;
			if( strcmp(node->binary_op.op, "<")==0 ) return L < R;
			if( strcmp(node->binary_op.op, ">")==0 ) return L > R;


			break;
		}

		case NODE_UNARY_OP: {
			if( strcmp(node->unary_op.op,"NOT")==0 || strcmp(node->unary_op.op,"not")==0 ){
				int child = evaluate_formula(node->unary_op.child);
				return !child;

			}
		}

		default:
			// no other node types should appear in a formula
			fprintf(stderr, "evaluate_formula: unexpected node type %d\n", node->type);
			break;


	}

	return 0;
}


// 1) Arithmetic evaluator
int evaluate_expr (ASTNode* node) {

	switch (node->type) {
		case NODE_NUMBER: {
			return node->number;
			break;
		}

		case NODE_BIN_OP: {
			int L = evaluate_expr(node->binary_op.left);
			int R = evaluate_expr(node->binary_op.right);
			if (strcmp(node->binary_op.op, "+")==0)    return L + R;
			if (strcmp(node->binary_op.op, "-")==0)    return L - R;
			if (strcmp(node->binary_op.op, "*")==0)    return L * R;
			if (strcmp(node->binary_op.op, "/")==0)    return L / R;

			break;
		}

		case NODE_FUNCTION: {
			int arg1 = evaluate_expr(node->function.arg1);
			int arg2 = evaluate_expr(node->function.arg2);

			if( strcmp(node->function.fname,"min")==0 ){
				return (arg1 < arg2) ? arg1 : arg2;
			}

			if( strcmp(node->function.fname,"max")==0 ){
				return (arg1 < arg2) ? arg2 : arg1;
			}

		}
	}
	
}