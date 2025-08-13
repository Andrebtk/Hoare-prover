#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"



ASTNode* create_node_binary(char* type, ASTNode* left, ASTNode* right) {
    ASTNode* res = calloc(1, sizeof(ASTNode));
    res->type = NODE_BIN_OP;

    res->binary_op.op = strdup(type);
    res->binary_op.left = left;
    res->binary_op.right = right;
    return res;
}

ASTNode* create_node_unary(char* type, ASTNode* child) {
	ASTNode* res = calloc(1, sizeof(ASTNode));
	res->type = NODE_UNARY_OP;

	res->unary_op.op = strdup(type);
	res->unary_op.child = child;

	return res;
}

ASTNode* create_node_number(int num) {
    ASTNode* res = calloc(1, sizeof(ASTNode));
    res->type = NODE_NUMBER;

    res->number = num;
    return res;
}

ASTNode* create_node_id(char *input) {
    ASTNode* res = calloc(1, sizeof(ASTNode));
    res->type = NODE_ID;

    res->id_name = strdup(input);
    return res;
}

ASTNode* create_node_assign(char* id, ASTNode* expr){
    ASTNode* res = calloc(1, sizeof(ASTNode));
    res->type = NODE_ASSIGN;
    res->Assign.id = strdup(id);
    res->Assign.expr = expr;
    return res;
}

ASTNode* create_node_If_Else(ASTNode* condition, DLL* block_if, DLL* block_else) {
    ASTNode* res = calloc(1, sizeof(ASTNode));
    res->type = NODE_IF_ELSE;

    res->If.condition = condition;
    res->If.block_if = block_if;
	res->If.block_else = block_else;
    return res;
}

ASTNode* create_node_While(ASTNode* condition, DLL* block, ASTNode* invariant, ASTNode* variant) {
    ASTNode* res = calloc(1, sizeof(ASTNode));
    res->type = NODE_WHILE;

    res->While.condition = condition;
    res->While.block_main = block;
	res->While.invariant = invariant;
	res->While.variant = variant;
	return res;
}

ASTNode* create_node_Func(const char* name, ASTNode* a1, ASTNode* a2) {
	ASTNode* res = calloc(1, sizeof(ASTNode));
	res->type = NODE_FUNCTION;

	res->function.fname = strdup(name);
	res->function.arg1 = a1;
	res->function.arg2 = a2;
	return res;
}

ASTNode* create_node_bool(int value) {
    ASTNode* node = calloc(1, sizeof(ASTNode));
    node->type = NODE_BOOL;
    node->bool_value = value;
    return node;
}




DLL* create_DLL() {
    DLL* res = malloc(sizeof(DLL));
    res->first = NULL;
    res->last = NULL;
	res->pre = NULL;
	res->post = NULL;
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
            print_ASTNode(node->If.condition, -1, prof+1);
			
			printf("\n");
			print_prof(prof);
            printf("block IF : \n");

            //print_prof(prof+1);
			print_DLL(node->If.block_if, prof+1, -1);
			printf("\n");

			print_prof(prof);
            printf("block Else : \n");

            //print_prof(prof+1);
			print_DLL(node->If.block_else, prof+1, -1);
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
			if (node->function.arg2) {
				print_ASTNode(node->function.arg2, -1, prof+1);
			}
			else {
				print_prof(prof+1);
				printf("NULL\n");
			}

			break;
		}

        case NODE_WHILE: {
			
			print_prof(prof-1);
			print_line(iter);
			print_prof(prof);
          	printf("Node WHILE:\n");
			
			print_prof(prof);
            printf("condition : \n");
            print_ASTNode(node->While.condition, -1, prof+1);

			print_prof(prof);
            printf("Invariant : \n");
            print_ASTNode(node->While.invariant, -1, prof+1);

			print_prof(prof);
            printf("Variant : \n");
            print_ASTNode(node->While.variant, -1, prof+1);


			printf("\n");
			print_prof(prof);
            printf("block : \n");
            print_DLL(node->While.block_main, prof+1, -1);
			printf("\n");
            break;
			
        };

		case NODE_BOOL: {
			print_prof(prof);
			print_line(iter);
			printf("Node Bool: \n");
			print_prof(prof);
			if(node->bool_value == 0) 	{ printf("False \n"); }
			else 						{ printf("True \n"); }

			printf("\n");
			break;
		}
		

		case NODE_UNARY_OP: {
			print_prof(prof);
			print_line(iter);
			printf("Node Unary: %s \n", node->unary_op.op);

			print_prof(prof);
			printf("Child : \n");
			print_ASTNode(node->unary_op.child, -1, prof+1);
			break;
		}
			
        
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
void print_AST_formula(ASTNode* node) {
    if (!node) return;

    switch(node->type) {
        case NODE_BIN_OP:
            printf("(");
            print_AST_formula(node->binary_op.left);
            printf(" %s ", node->binary_op.op);
            print_AST_formula(node->binary_op.right);
            printf(")");
            break;
        case NODE_UNARY_OP:
            printf("(%s ", node->unary_op.op);
            print_AST_formula(node->unary_op.child);
            printf(")");
            break;
        case NODE_NUMBER:
            printf("%d", node->number);
            break;
        case NODE_ID:
            printf("%s", node->id_name);
            break;
        case NODE_BOOL:
            printf("%s", node->bool_value ? "true" : "false");
            break;
        case NODE_FUNCTION:  // ADD THIS CASE
            printf("%s(", node->function.fname);
            if (node->function.arg1) {
                print_AST_formula(node->function.arg1);
                if (node->function.arg2) {
                    printf(", ");
                    print_AST_formula(node->function.arg2);
                }
            }
            printf(")");
            break;
        default:
            printf("<unknown>");
    }
}





ASTNode* substitute(ASTNode* formula, const char* var, ASTNode* replacement) {
	if (formula == NULL || var == NULL || replacement == NULL) return NULL;

	switch (formula->type) {
		case NODE_ASSIGN: {
			ASTNode* new_expr = substitute(formula->Assign.expr, var, replacement);
			return create_node_assign(formula->Assign.id, new_expr);
		}

		case NODE_BIN_OP: {
			ASTNode* left = substitute(formula->binary_op.left, var, replacement);
			ASTNode* right = substitute(formula->binary_op.right, var, replacement);
			return create_node_binary(formula->binary_op.op, left, right);
		}

		case NODE_UNARY_OP: {
			ASTNode* new_child = substitute(formula->unary_op.child, var, replacement);
			return create_node_unary(formula->unary_op.op, new_child);
		}

		case NODE_IF_ELSE: {
			ASTNode* new_condition = substitute(formula->If.condition, var, replacement);
			
			// Substitute in the IF block
			DLL* new_block_if = create_DLL();
			if (formula->If.block_if) {
				line_linkedlist* cur = formula->If.block_if->first;
				while (cur != NULL) {
					ASTNode* new_node = substitute(cur->node, var, replacement);
					DLL_append(new_block_if, new_node);
					cur = cur->next;
				}
			}
			
			// Substitute in the ELSE block
			DLL* new_block_else = create_DLL();
			if (formula->If.block_else) {
				line_linkedlist* cur = formula->If.block_else->first;
				while (cur != NULL) {
					ASTNode* new_node = substitute(cur->node, var, replacement);
					DLL_append(new_block_else, new_node);
					cur = cur->next;
				}
			}
			
			return create_node_If_Else(new_condition, new_block_if, new_block_else);
		}

		case NODE_WHILE: {
			ASTNode* new_condition = substitute(formula->While.condition, var, replacement);
			ASTNode* new_invariant = substitute(formula->While.invariant, var, replacement);
			ASTNode* new_variant = substitute(formula->While.variant, var, replacement);
			
			// Substitute in the DLL block_main
			DLL* new_block = create_DLL();
			if (formula->While.block_main) {
				line_linkedlist* cur = formula->While.block_main->first;
				while (cur != NULL) {
					ASTNode* new_node = substitute(cur->node, var, replacement);
					DLL_append(new_block, new_node);
					cur = cur->next;
				}
			}
			
			return create_node_While(new_condition, new_block, new_invariant, new_variant);
		}

		case NODE_NUMBER: {
			return create_node_number(formula->number);
		}

		case NODE_BOOL: {
			// Missing case! This was causing issues
			return create_node_bool(formula->bool_value);
		}

		case NODE_ID: {
			if (strcmp(formula->id_name, var) == 0) {
				return clone_node(replacement);
			} else {
				return create_node_id(formula->id_name);
			}
		}

		case NODE_FUNCTION: {
			ASTNode* new_arg1 = substitute(formula->function.arg1, var, replacement);
			ASTNode* new_arg2 = formula->function.arg2 ? 
				substitute(formula->function.arg2, var, replacement) : NULL;
			return create_node_Func(formula->function.fname, new_arg1, new_arg2);
		}

		default:
			// For unknown types, just clone the node
			return clone_node(formula);
	}
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
				orig->unary_op.op,
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
				orig->function.fname,
				clone_node(orig->function.arg1),
				clone_node(orig->function.arg2)
			);

			break;
		}

		case NODE_IF_ELSE: {
			// Clone the “then” block
			DLL* then_copy = create_DLL();
			for (line_linkedlist* it = orig->If.block_if->first; it; it = it->next) {
				DLL_append(then_copy, clone_node(it->node));
			}
			// Clone the “else” block
			DLL* else_copy = create_DLL();
			for (line_linkedlist* it = orig->If.block_else->first; it; it = it->next) {
				DLL_append(else_copy, clone_node(it->node));
			}
			return create_node_If_Else(
				clone_node(orig->If.condition),
				then_copy,
				else_copy
			);

			break;
		}

		case NODE_WHILE: {
			
			DLL* body_copy = create_DLL();
			for (line_linkedlist* it = orig->While.block_main->first; it; it = it->next) {
				DLL_append(body_copy, clone_node(it->node));
			}
			return create_node_While(
				clone_node(orig->While.condition),
				body_copy,
				clone_node(orig->While.invariant),
				clone_node(orig->While.variant)
			);
			
			break;
		}
	}
}

void free_ll(line_linkedlist* l) {
	while(l) {
		line_linkedlist* next = l->next;
		free_ASTNode(l->node);
		free(l);
		l = next;
	}
}


void free_DLL(DLL* l) {
	if(l==NULL) return;

	free_ASTNode(l->pre);
	free_ASTNode(l->post);

	free_ll(l->first);
	free(l);
}


void free_ASTNode(ASTNode* node) {
	if (node == NULL) return ;

	switch (node->type) {
		case NODE_ASSIGN: {
			if(node->Assign.id) free(node->Assign.id);
			if(node->Assign.expr) free_ASTNode(node->Assign.expr);
			break;
		}

		case NODE_FUNCTION: {
			if(node->function.fname) free(node->function.fname);
			if(node->function.arg1) free_ASTNode(node->function.arg1);
			if(node->function.arg2) free_ASTNode(node->function.arg2);
			break;
		}

		case NODE_IF_ELSE: {
			if(node->If.condition) free_ASTNode(node->If.condition);
			if(node->If.block_if) free_DLL(node->If.block_if);
			if(node->If.block_else) free_DLL(node->If.block_else);
			break;
		}

		case NODE_WHILE: {
			if(node->While.condition) free_ASTNode(node->While.condition);
			if(node->While.invariant) free_ASTNode(node->While.invariant);
			if(node->While.variant) free_ASTNode(node->While.variant);
			if(node->While.block_main) free_DLL(node->While.block_main);
			break;
		}

		case NODE_BIN_OP: {
			if(node->binary_op.op) free(node->binary_op.op);
			if(node->binary_op.left) free_ASTNode(node->binary_op.left);
			if(node->binary_op.right) free_ASTNode(node->binary_op.right);
			break;
		}

		case NODE_UNARY_OP: {
			if(node->unary_op.op) free(node->unary_op.op);
			if(node->unary_op.child) free_ASTNode(node->unary_op.child);
			break;
		}

		case NODE_ID: {
			if(node->id_name) free(node->id_name);
			break;
		}

		case NODE_NUMBER:
		case NODE_BOOL:
			// Nothing extra to free
			break;

		default:
			// Unknown type; maybe log a warning
			fprintf(stderr, "Warning: freeing unknown ASTNode type %d\n", node->type);
			break;

	}

	free(node);

}