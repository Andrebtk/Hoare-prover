#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <z3.h>

#include "data-struct.h"

Z3_func_decl fact_func = NULL;  // définition globale

int fact(int n) {

	if (n<0)
		return -1;

	if( (n==0) || (n==1) )
		return 1;

	return n * fact(n-1);
	
}


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

    res->If.condition = condition;
    res->If.block_if = block_if;
	res->If.block_else = block_else;
    return res;
}

ASTNode* create_node_While(ASTNode* condition, DLL* block, ASTNode* invariant, ASTNode* variant) {
    ASTNode* res = malloc(sizeof(ASTNode));
    res->type = NODE_WHILE;

    res->While.condition = condition;
    res->While.block_main = block;
	res->While.invariant = invariant;
	res->While.variant = variant;
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
			ASTNode* new_condition = substitute(formula->While.condition, var, replacement);
			ASTNode* new_invariant = substitute(formula->While.invariant, var, replacement);
			ASTNode* new_variant = substitute(formula->While.variant, var, replacement);
			
			// Substitute in the DLL block_main: create a new DLL with substituted statements
			DLL* new_block = create_DLL();
			line_linkedlist* cur = formula->While.block_main->first;
			while (cur != NULL) {
				ASTNode* new_node = substitute(cur->node, var, replacement);
				DLL_append(new_block, new_node);
				cur = cur->next;
			}
			
			return create_node_While(new_condition, new_block, new_invariant, new_variant);
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

		case NODE_WHILE: {
			return hoare_WhileRule(node, post);
			break;
		}

		default:
			fprintf(stderr, "hoare_statement: unsupported node type %d\n", node->type);
			return NULL;

	}

}

ASTNode* hoare_AssignmentRule(ASTNode* node, ASTNode* post /*DLL* code*/) {
	return substitute(post, 
						node->Assign.id, 
						node->Assign.expr);
}


/*
	{P} if B then { C1 } else { C2 } {Q}
*/
ASTNode* hoare_IfElseRule(ASTNode* node_IfElse, ASTNode* post) {

	if (!node_IfElse || node_IfElse->type != NODE_IF_ELSE) {
		fprintf(stderr, "Invalid node in hoare_IfElseRule\n");
		return NULL;
	}

	
	ASTNode* condition = node_IfElse->If.condition;	// B
	DLL* block_if = node_IfElse->If.block_if;		// C1
	DLL* block_else = node_IfElse->If.block_else;		// C2

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


/*
	{I} while B do C {I ∧ ¬B}
*/
ASTNode* hoare_WhileRule(ASTNode* node, ASTNode* post) {
	ASTNode* condition = node->While.condition; // B
	ASTNode* invariant = node->While.invariant; // I
	ASTNode* variant = node->While.variant;
	DLL* block_code = node->While.block_main;   // C


	ASTNode* notB = create_node_unary("not", clone_node(condition));
	ASTNode* I_and_notB  = create_node_binary("and", clone_node(invariant), notB);
	ASTNode* I_and_B = create_node_binary("and", clone_node(invariant), clone_node(condition));

	ASTNode* wp_body = hoare_prover(block_code, I_and_B, clone_node(invariant));
	
	
	ASTNode* right = create_node_binary("->", I_and_notB, clone_node(post));
	ASTNode* left = create_node_binary("->", I_and_B, wp_body);

	// partial correctness: ((I ∧ B) -> wp_body) ∧ ((I ∧ ¬B) -> post)
	ASTNode* partial_correctness = create_node_binary("and", left, right);


	// === total correctness additional checks ===
	ASTNode* variant_after = clone_node(variant);
	line_linkedlist* cur = block_code->first;
	while (cur != NULL) {
		if (cur->node->type == NODE_ASSIGN) {
			variant_after = substitute(variant_after, cur->node->Assign.id, cur->node->Assign.expr);
		}
		cur = cur->next;
	}

	//create: v' < V
	ASTNode* variant_decreases = create_node_binary("<", variant_after, clone_node(variant));

	//Create: v >= 0
	ASTNode* node_zero = create_node_number(0);
	ASTNode* variant_nonnegative = create_node_binary(">=", clone_node(variant), node_zero);

	// total correctness: (I ∧ B) -> (variant_after < variant ∧ 0 <= variant)
	ASTNode* decrease_condition = create_node_binary("and", variant_decreases, variant_nonnegative);
	ASTNode* termination_condition = create_node_binary("->", I_and_B, decrease_condition);
	
	return create_node_binary("and", partial_correctness, termination_condition);
}



// 1) Logical & comparison evaluator
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
			if( strcmp(node->binary_op.op, ">=")==0 ) return L >= R;
			if( strcmp(node->binary_op.op, "<=")==0 ) return L <= R;


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


// 2) Arithmetic evaluator
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

			int arg2 = -1;
			if(node->function.arg2 != NULL) {
				arg2 = evaluate_expr(node->function.arg2);
			}
			


			if( strcmp(node->function.fname,"min")==0 ){
				return (arg1 < arg2) ? arg1 : arg2;
			}

			if( strcmp(node->function.fname,"max")==0 ){
				return (arg1 < arg2) ? arg2 : arg1;
			}

			if ( strcmp(node->function.fname, "fact")==0 ) {
				return fact(arg1);
			}

		}
	}
	
}



void init_z3(Z3_context ctx) {
    Z3_sort int_sort = Z3_mk_int_sort(ctx);
    Z3_symbol fact_name = Z3_mk_string_symbol(ctx, "fact");

    // Création de la fonction récursive fact: Int -> Int
    fact_func = Z3_mk_rec_func_decl(ctx, fact_name, 1, &int_sort, int_sort);

    // Variable liée n (bound var)
    Z3_ast n = Z3_mk_bound(ctx, 0, int_sort);
    Z3_ast zero = Z3_mk_int(ctx, 0, int_sort);
    Z3_ast one = Z3_mk_int(ctx, 1, int_sort);
    Z3_ast n_minus_one = Z3_mk_sub(ctx, 2, (Z3_ast[]){n, one});
    Z3_ast fact_n_minus_one = Z3_mk_app(ctx, fact_func, 1, &n_minus_one);

    // corps récursif : if n == 0 then 1 else n * fact(n-1)
    Z3_ast body = Z3_mk_ite(ctx,
                            Z3_mk_eq(ctx, n, zero),
                            one,
                            Z3_mk_mul(ctx, 2, (Z3_ast[]){n, fact_n_minus_one}));

    // Ajout de la définition récursive
    Z3_add_rec_def(ctx, fact_func, 1, &n, body);
}



Z3_ast ast_to_z3(Z3_context ctx, ASTNode* node, HashMap* var_cache) {

	if (!node) {
		fprintf(stderr, "ast_to_z3: NULL node\n");
		return NULL;
	}

	//type int and bool
	static Z3_sort int_sort = NULL;
	static Z3_sort bool_sort = NULL;

	if (!int_sort) int_sort = Z3_mk_int_sort(ctx);
	if (!bool_sort) bool_sort = Z3_mk_bool_sort(ctx);

	if (!node) {
		fprintf(stderr, "ast_to_z3: NULL node\n");
		return NULL;
	}

	switch (node->type) {
		case NODE_NUMBER: {
			return Z3_mk_int(ctx, node->number, int_sort);

			break;
		}

		case NODE_BOOL: {
			return (node->bool_value) ? Z3_mk_true(ctx) : Z3_mk_false(ctx);

			break;
		}

		case NODE_ID: {
			if (!var_cache) {
				fprintf(stderr, "Error: var_cache is NULL\n");
				return NULL;
			}
			
			int index = hash(var_cache, node->id_name);
			HashEntry* entry = var_cache->table[index];

			while (entry) {
				if( strcmp(entry->key, node->id_name)==0 ){
					return (Z3_ast) entry->value;
				}
				entry = entry->next;
			}

			// Not found
			// create new Z3 variable and inser it in var_cache
			Z3_symbol sym = Z3_mk_string_symbol(ctx, node->id_name);
			Z3_ast var = Z3_mk_const(ctx, sym, int_sort);
			insert_HashMap(var_cache, node->id_name, var);
			return var;
			break;
		}

		case NODE_BIN_OP: {
			Z3_ast left = ast_to_z3(ctx, node->binary_op.left, var_cache);
			Z3_ast right = ast_to_z3(ctx, node->binary_op.right, var_cache);

			if (!left || !right) {
				fprintf(stderr, "ast_to_z3: NULL child in binary op\n");
				return NULL;
			}

			Z3_ast args[2] = {left, right};

			if ( strcmp(node->binary_op.op,"+")==0 ) { return Z3_mk_add(ctx, 2, args); }
			if ( strcmp(node->binary_op.op,"-")==0 ) { return Z3_mk_sub(ctx, 2, args); }
			if ( strcmp(node->binary_op.op,"*")==0 ) { return Z3_mk_mul(ctx, 2, args); }
			if ( strcmp(node->binary_op.op,"/")==0 ) { return Z3_mk_div(ctx, left, right); }
			if ( strcmp(node->binary_op.op,"<")==0 ) { return Z3_mk_lt(ctx, left, right); }
			if ( strcmp(node->binary_op.op,">")==0 ) { return Z3_mk_gt(ctx, left, right); }
			if ( strcmp(node->binary_op.op,"==")==0 ) { return Z3_mk_eq(ctx, left, right); }
			if ( strcmp(node->binary_op.op,"!=")==0 ) { return Z3_mk_distinct(ctx, 2, args); }
			if (strcmp(node->binary_op.op, "and") == 0) { return Z3_mk_and(ctx, 2, args); }
			if (strcmp(node->binary_op.op, ">=") == 0) { return Z3_mk_ge(ctx, left, right);	}
			if (strcmp(node->binary_op.op, "<=") == 0) { return Z3_mk_le(ctx, left, right); }
			if (strcmp(node->binary_op.op, "->") == 0) { return Z3_mk_implies(ctx, left, right); }

			fprintf(stderr, "ast_to_z3: Unknown binary op '%s'\n", node->binary_op.op);
			return NULL;
			break;
		}

		case NODE_UNARY_OP: {
			Z3_ast child = ast_to_z3(ctx, node->unary_op.child, var_cache);
			
			if (!child) {
				fprintf(stderr, "ast_to_z3: NULL child in unary op\n");
				return NULL;
			}

			if (strcmp(node->unary_op.op, "not") == 0) {
				return Z3_mk_not(ctx, child);
			}
			
			fprintf(stderr, "ast_to_z3: Unknown unary op '%s'\n", node->unary_op.op);
			return NULL;
		}

		case NODE_FUNCTION: {
			if (strcmp(node->function.fname, "fact") == 0) {
				// Vérifie que fact_func est initialisé
				extern Z3_func_decl fact_func; // ou autre moyen de le passer
				if (!fact_func) {
					fprintf(stderr, "ast_to_z3: fact_func not initialized\n");
					return NULL;
				}
				Z3_ast arg = ast_to_z3(ctx, node->function.arg1, var_cache);
				if (!arg) {
					fprintf(stderr, "ast_to_z3: NULL argument to fact function\n");
					return NULL;
				}
				return Z3_mk_app(ctx, fact_func, 1, &arg);
			}
			fprintf(stderr, "ast_to_z3: Unknown function '%s'\n", node->function.fname);
			return NULL;
		}


		default: {
			printf("Unsupported AST node type in Z3 converter: %d\n", node->type);
			return NULL;
		}
	}
}