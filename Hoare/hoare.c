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
    /* hoare_prover takes ownership of 'post' */
    ASTNode* wp = post;

    while (current != NULL) {
        ASTNode* new_wp = hoare_statement(current->node, wp);
        /* hoare_statement is expected to return a NEW node (the new wp)
           and not depend on 'wp' after return, so we free the old wp here. */
        free_ASTNode(wp);
        wp = new_wp;
        current = current->prec;
    }

    return wp; /* caller must free the returned wp when done */
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

ASTNode* hoare_AssignmentRule(ASTNode* node, ASTNode* post ) {
	ASTNode* post_clone = clone_node(post);
	ASTNode* result = substitute(post_clone, node->Assign.id, node->Assign.expr);
	free_ASTNode(post_clone); // free the clone, safe
	return result;
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
	DLL* block_else = node_IfElse->If.block_else;	// C2 (can be NULL)

	if (!condition || !block_if) {
		fprintf(stderr, RED "Incomplete IF node (missing condition or if-block)\n" RESET);
		return NULL;
	}

	// Clone post for separate branches
	ASTNode* post_clone_if = clone_node(post);
	ASTNode* wp_if = hoare_prover(block_if, NULL, post_clone_if);

	ASTNode* wp_else;
	if (block_else && block_else->first) {
		// There is an else block
		ASTNode* post_clone_else = clone_node(post);
		wp_else = hoare_prover(block_else, NULL, post_clone_else);
	} else {
		// No else block - the postcondition should hold directly
		wp_else = clone_node(post);
	}

	// Build implication nodes
	ASTNode* left = create_node_binary("->", clone_node(condition), wp_if);
	ASTNode* right = create_node_binary("->", create_node_unary("not", clone_node(condition)), wp_else);

	ASTNode* result = create_node_binary("and", left, right);
	return result;
}


/*
	{I} while B do C {I ∧ ¬B}
*/
ASTNode* hoare_WhileRule(ASTNode* node, ASTNode* post) {
    ASTNode* condition = node->While.condition;
    ASTNode* invariant = node->While.invariant;
    ASTNode* variant = node->While.variant;
    DLL* block_code = node->While.block_main;

    ASTNode* I_and_B = create_node_binary("and", clone_node(invariant), clone_node(condition));
    ASTNode* wp_body = hoare_prover(block_code, I_and_B, clone_node(invariant));

    ASTNode* I_and_notB = create_node_binary("and", clone_node(invariant),
                                             create_node_unary("not", clone_node(condition)));
    ASTNode* right = create_node_binary("->", I_and_notB, clone_node(post));
    ASTNode* left = create_node_binary("->", I_and_B, wp_body);

    ASTNode* partial_correctness = create_node_binary("and", left, right);

    // Total correctness: variant checks
    ASTNode* variant_after = clone_node(variant);
    line_linkedlist* cur = block_code->first;
    while (cur != NULL) {
        if (cur->node->type == NODE_ASSIGN) {
            ASTNode* tmp = substitute(variant_after, cur->node->Assign.id, cur->node->Assign.expr);
            free_ASTNode(variant_after);  // free old
            variant_after = tmp;
        }
        cur = cur->next;
    }

    ASTNode* variant_decreases = create_node_binary("<", variant_after, clone_node(variant));
    ASTNode* variant_nonnegative = create_node_binary(">=", clone_node(variant), create_node_number(0));
    ASTNode* decrease_condition = create_node_binary("and", variant_decreases, variant_nonnegative);
    
    // FIX: Clone I_and_B instead of reusing it (already used in partial_correctness)
    ASTNode* I_and_B_clone = create_node_binary("and", clone_node(invariant), clone_node(condition));
    ASTNode* termination_condition = create_node_binary("->", I_and_B_clone, decrease_condition);

    ASTNode* result = create_node_binary("and", partial_correctness, termination_condition);
    return result;
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
