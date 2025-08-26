#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode* alloc_node(NodeType type) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    memset(node, 0, sizeof(ASTNode));
    node->type = type;
    return node;
}

ASTNode* create_node_binary(char* type, ASTNode* left, ASTNode* right) {
     ASTNode* res = alloc_node(NODE_BIN_OP);

    res->binary_op.op = strdup(type);
    res->binary_op.left = left;
    res->binary_op.right = right;
    return res;
}

ASTNode* create_node_unary(char* type, ASTNode* child) {
	ASTNode* res = alloc_node(NODE_UNARY_OP);

	res->unary_op.op = strdup(type);
	res->unary_op.child = child;

	return res;
}


ASTNode* create_node_number(int num) {
	ASTNode* res = alloc_node(NODE_NUMBER);

	res->number = num;
	return res;
}

ASTNode* create_node_id(char *input) {
	ASTNode* res = alloc_node(NODE_ID);

	res->id_name = strdup(input);
	return res;
}


ASTNode* create_node_assign(char* id, ASTNode* expr){
	ASTNode* res = alloc_node(NODE_ASSIGN);
	res->Assign.id = strdup(id);
	res->Assign.expr = expr;
	return res;
}

ASTNode* create_node_If_Else(ASTNode* condition, DLL* block_if, DLL* block_else) {
	ASTNode* res = alloc_node(NODE_IF_ELSE);

	res->If.condition = condition;
	res->If.block_if = block_if;
	res->If.block_else = block_else;
	return res;
}

ASTNode* create_node_While(ASTNode* condition, DLL* block, ASTNode* invariant, ASTNode* variant) {
	ASTNode* res = alloc_node(NODE_WHILE);

	res->While.condition = condition;
	res->While.block_main = block;
	res->While.invariant = invariant;
	res->While.variant = variant;
	return res;
}

ASTNode* create_node_Func(const char* name, ASTNode* a1, ASTNode* a2) {
	ASTNode* res = alloc_node(NODE_FUNCTION);

	res->function.fname = strdup(name);
	res->function.arg1 = a1;
	res->function.arg2 = a2;
	return res;
}

ASTNode* create_node_bool(int value) {
	ASTNode* res = alloc_node(NODE_BOOL);

	res->bool_value = value;
	return res;
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
			if (node->function.arg2)
				print_ASTNode(node->function.arg2, -1, prof+1);
			else
				print_prof(prof+1);
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

DLL* clone_DLL(const DLL* src) {
    if (!src) return NULL;
    DLL* out = (DLL*)malloc(sizeof(DLL));
    if (!out) { perror("malloc"); exit(1); }

    out->pre  = clone_node(src->pre);
    out->post = clone_node(src->post);
    out->first = out->last = NULL;

    line_linkedlist* prev = NULL;
    for (line_linkedlist* cur = src->first; cur; cur = cur->next) {
        line_linkedlist* nln = (line_linkedlist*)malloc(sizeof(line_linkedlist));
        if (!nln) { perror("malloc"); exit(1); }
        nln->node = clone_node(cur->node);
        nln->next = NULL;
        nln->prec = prev;
        if (prev) prev->next = nln;
        else out->first = nln;
        prev = nln;
    }
    out->last = prev;
    return out;
}

ASTNode* clone_node(const ASTNode* src) {
    if (!src) return NULL;

    ASTNode* dst = alloc_node(src->type);

    switch (src->type) {
        case NODE_ASSIGN:
            dst->Assign.id   = src->Assign.id ? strdup(src->Assign.id) : NULL;
            dst->Assign.expr = clone_node(src->Assign.expr);
            break;

        case NODE_FUNCTION:
            dst->function.fname = src->function.fname ? strdup(src->function.fname) : NULL;
            dst->function.arg1  = clone_node(src->function.arg1);
            dst->function.arg2  = clone_node(src->function.arg2);
            break;

        case NODE_IF_ELSE:
            dst->If.condition  = clone_node(src->If.condition);
            dst->If.block_if   = clone_DLL(src->If.block_if);
            dst->If.block_else = clone_DLL(src->If.block_else);
            break;

        case NODE_WHILE:
            dst->While.condition  = clone_node(src->While.condition);
            dst->While.invariant  = clone_node(src->While.invariant);
            dst->While.variant    = clone_node(src->While.variant);
            dst->While.block_main = clone_DLL(src->While.block_main);
            break;

        case NODE_BIN_OP:
            dst->binary_op.op    = src->binary_op.op ? strdup(src->binary_op.op) : NULL;
            dst->binary_op.left  = clone_node(src->binary_op.left);
            dst->binary_op.right = clone_node(src->binary_op.right);
            break;

        case NODE_UNARY_OP:
            dst->unary_op.op    = src->unary_op.op ? strdup(src->unary_op.op) : NULL;
            dst->unary_op.child = clone_node(src->unary_op.child);
            break;

        case NODE_ID:
            dst->id_name = src->id_name ? strdup(src->id_name) : NULL;
            break;

        case NODE_NUMBER:
            dst->number = src->number;
            break;

        case NODE_BOOL:
            dst->bool_value = src->bool_value;
            break;

        default:
            fprintf(stderr, "clone_node: unknown type %d\n", src->type);
            break;
    }

    return dst;
}

DLL* substitute_DLL(const DLL* src, const char* id, const ASTNode* repl) {
    if (!src) return NULL;
    DLL* out = (DLL*)malloc(sizeof(DLL));
    if (!out) { perror("malloc"); exit(1); }

    out->pre  = substitute(src->pre, id, repl);
    out->post = substitute(src->post, id, repl);
    out->first = out->last = NULL;

    line_linkedlist* prev = NULL;
    for (line_linkedlist* cur = src->first; cur; cur = cur->next) {
        line_linkedlist* nln = (line_linkedlist*)malloc(sizeof(line_linkedlist));
        if (!nln) { perror("malloc"); exit(1); }

        nln->node = substitute(cur->node, id, repl);
        nln->next = NULL;
        nln->prec = prev;
        if (prev) prev->next = nln;
        else out->first = nln;
        prev = nln;
    }
    out->last = prev;
    return out;
}



ASTNode* substitute(const ASTNode* node, const char* id, const ASTNode* repl) {
    if (!node) return NULL;
    if (!id) return clone_node(node);

    switch (node->type) {
        case NODE_ID:
            if (node->id_name && strcmp(node->id_name, id) == 0)
                return clone_node(repl);
            else
                return clone_node(node);

        case NODE_NUMBER:
        case NODE_BOOL:
            return clone_node(node);

        case NODE_ASSIGN: {
            ASTNode* out = alloc_node(NODE_ASSIGN);
            out->Assign.id   = node->Assign.id ? strdup(node->Assign.id) : NULL;
            out->Assign.expr = substitute(node->Assign.expr, id, repl);
            return out;
        }

        case NODE_FUNCTION: {
            ASTNode* out = alloc_node(NODE_FUNCTION);
            out->function.fname = node->function.fname ? strdup(node->function.fname) : NULL;
            out->function.arg1  = substitute(node->function.arg1, id, repl);
            out->function.arg2  = substitute(node->function.arg2, id, repl);
            return out;
        }

        case NODE_BIN_OP: {
            ASTNode* out = alloc_node(NODE_BIN_OP);
            out->binary_op.op    = node->binary_op.op ? strdup(node->binary_op.op) : NULL;
            out->binary_op.left  = substitute(node->binary_op.left, id, repl);
            out->binary_op.right = substitute(node->binary_op.right, id, repl);
            return out;
        }

        case NODE_UNARY_OP: {
            ASTNode* out = alloc_node(NODE_UNARY_OP);
            out->unary_op.op    = node->unary_op.op ? strdup(node->unary_op.op) : NULL;
            out->unary_op.child = substitute(node->unary_op.child, id, repl);
            return out;
        }

       case NODE_IF_ELSE: {
			ASTNode* out = alloc_node(NODE_IF_ELSE);
			out->If.condition  = substitute(node->If.condition, id, repl);

			// Substitue récursivement dans les DLL
			out->If.block_if   = node->If.block_if   ? substitute_DLL(node->If.block_if, id, repl) : NULL;
			out->If.block_else = node->If.block_else ? substitute_DLL(node->If.block_else, id, repl) : NULL;
			return out;
		}

		case NODE_WHILE: {
			ASTNode* out = alloc_node(NODE_WHILE);
			out->While.condition  = substitute(node->While.condition, id, repl);
			out->While.invariant  = substitute(node->While.invariant, id, repl);
			out->While.variant    = substitute(node->While.variant, id, repl);
			out->While.block_main = node->While.block_main ? substitute_DLL(node->While.block_main, id, repl) : NULL;
			return out;
		}

        default:
            return clone_node(node);
    }
}

void free_ASTNode(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_ASSIGN:
            free(node->Assign.id);
            free_ASTNode(node->Assign.expr);
            break;

        case NODE_FUNCTION:
            free(node->function.fname);
            free_ASTNode(node->function.arg1);
            free_ASTNode(node->function.arg2);
            break;

        case NODE_IF_ELSE:
            free_ASTNode(node->If.condition);
            free_DLL(node->If.block_if);
            free_DLL(node->If.block_else);
            break;

        case NODE_WHILE:
            free_ASTNode(node->While.condition);
            free_ASTNode(node->While.invariant);
            free_ASTNode(node->While.variant);
            free_DLL(node->While.block_main);
            break;

        case NODE_BIN_OP:
            free(node->binary_op.op);
            free_ASTNode(node->binary_op.left);
            free_ASTNode(node->binary_op.right);
            break;

        case NODE_UNARY_OP:
            free(node->unary_op.op);
            free_ASTNode(node->unary_op.child);
            break;

        case NODE_ID:
            free(node->id_name);
            break;

        case NODE_NUMBER:
        case NODE_BOOL:
            break;

        default:
            fprintf(stderr, "Warning: freeing unknown ASTNode type %d\n", node->type);
            break;
    }

    free(node);
}

void free_ll(line_linkedlist* l) {
    while (l) {
        line_linkedlist* next = l->next;
        free_ASTNode(l->node); // frees the ASTNode
        free(l);
        l = next;
    }
}

void free_DLL(DLL* dll) {
    if (!dll) return;
    free_ASTNode(dll->pre);
    free_ASTNode(dll->post);
    free_ll(dll->first); // safely frees all nodes in DLL
    free(dll);
}

