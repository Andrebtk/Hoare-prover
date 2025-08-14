#include "hoare.h"
#include <string.h>


int fact(int n) {

	if (n<0)
		return -1;

	if( (n==0) || (n==1) )
		return 1;

	return n * fact(n-1);
	
}


int is_node_true(ASTNode* node) {
	return (node != NULL) 
			&& (node->type == NODE_BOOL) 
			&& (node->bool_value == 1);
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
