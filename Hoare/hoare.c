#include "hoare.h"
#include <string.h>

// factorial used by evaluate_expr()
int fact(int n) {
	if (n<0) return -1;
	if( (n==0) || (n==1) ) return 1;
	return n * fact(n-1);
}

// Detect a literal boolean "true" AST node 
int is_node_true(ASTNode* node) {
	return (node != NULL) 
			&& (node->type == NODE_BOOL) 
			&& (node->bool_value == 1);
}


// Backward Hoare prover: compute precondition for whole DLL.
ASTNode* hoare_prover(DLL* code, ASTNode* pre, ASTNode* post) {

	if (!code || !post) {
		fprintf(stderr, RED "NULL input to hoare_prover\n" RESET);
		return NULL;
	}

	line_linkedlist *current = code->last;
	ASTNode* wp = clone_node(post); 

	while( current != NULL ) {
		ASTNode* new_wp = hoare_statement(current->node, wp);
		free_ASTNode(wp); 
		wp = new_wp;
		current = current->prec;
	}

	return wp;
}


// Dispatch a statement to the proper Hoare rule implementation.
ASTNode* hoare_statement(ASTNode* node, ASTNode* post) {
	switch (node->type) {

		case NODE_ASSIGN : 	return hoare_AssignmentRule(node, post); // Assignment axiom
		case NODE_IF_ELSE:	return hoare_IfElseRule(node, post); // Conditional rule
		case NODE_WHILE:	return hoare_WhileRule(node, post); // while rule (+variant checks)

		default:
			fprintf(stderr, "hoare_statement: unsupported node type %d\n", node->type);
			return NULL;
	}
}


/* ------------------------------------------------------------------
	Assignment rule (axiom schema):
		{ P[E/x] } x := E { P }
	Implementation:
		- compute a substitution of the postcondition: replace occurrences
		of the assigned id with the right-hand expression.
		- return the substituted AST (fresh allocation).
	 ------------------------------------------------------------------ */
ASTNode* hoare_AssignmentRule(ASTNode* node, ASTNode* post ) {
	if (!node || node->type != NODE_ASSIGN) {
		fprintf(stderr, "hoare_AssignmentRule: invalid node\n");
		return NULL;
	}

	ASTNode* post_clone = clone_node(post);
	ASTNode* result = substitute(post_clone, node->Assign.id, node->Assign.expr);
	free_ASTNode(post_clone); // free the clone, safe
	return result;
}


/* ------------------------------------------------------------------
	If-Else rule (conditional):

	Formal schema:
		If { B ∧ P } S { Q }  and  { ¬B ∧ P } T { Q }
		then { P } if B then S else T { Q }

	Implementation:
		- Compute wp_if = hoare_prover(block_if, NULL, clone(post)).
		- Compute wp_else = hoare_prover(block_else, NULL, clone(post)) or clone(post) if no else block.
		- Return (B -> wp_if) ∧ (¬B -> wp_else).
   ------------------------------------------------------------------ */
ASTNode* hoare_IfElseRule(ASTNode* node_IfElse, ASTNode* post) {

	if (!node_IfElse || node_IfElse->type != NODE_IF_ELSE) {
		fprintf(stderr, "Invalid node in hoare_IfElseRule\n");
		return NULL;
	}

	
	ASTNode* condition = node_IfElse->If.condition;		/* B */
	DLL* block_if = node_IfElse->If.block_if;			/* S */
	DLL* block_else = node_IfElse->If.block_else;		/* T (may be NULL) */

	if (!condition || !block_if || !block_else) {
		fprintf(stderr, RED "Incomplete IF node\n" RESET);
		return NULL;
	}

	/* For each branch we pass a clone of post to hoare_prover so the branch
		computation is independent and returns a fresh AST. */
	ASTNode* post_clone_if = clone_node(post);
	ASTNode* post_clone_else = clone_node(post);

	ASTNode* wp_if = hoare_prover(block_if, NULL, post_clone_if);
	ASTNode* wp_else = hoare_prover(block_else, NULL, post_clone_else);

	// Free the cloned post conditions since they're consumed by hoare_prover
	free_ASTNode(post_clone_if);
	free_ASTNode(post_clone_else);

	// Build implication nodes
	ASTNode* condition_clone = clone_node(condition);
	ASTNode* condition_not_clone = clone_node(condition);
	
	
	/* Build (B -> pre_if) */
	ASTNode* left = create_node_binary("->", condition_clone, wp_if);
	
	 /* Build (¬B -> pre_else) */
	ASTNode* not_condition = create_node_unary("not", condition_not_clone);
	ASTNode* right = create_node_binary("->", not_condition, wp_else);

	/* Return (B -> pre_if) ∧ (¬B -> pre_else) */
	ASTNode* result = create_node_binary("and", left, right);
	return result;
}


/* ------------------------------------------------------------------
	While rule (partial correctness + termination checks)

	FORMAL SCHEMA USED (partial correctness):
		{ P ∧ B } S { P }
		-----------------
		{ P } while B do S done { ¬B ∧ P }

	FORMAL SCHEMA FOR TOTAL CORRECTNESS (variant):
		If t is a variant and executing S under P∧B reduces t, 
		then the loop terminates.

		Concretely we encode:
			(I ∧ B) -> (variant_after < variant ∧ variant >= 0)
   ------------------------------------------------------------------ */
ASTNode* hoare_WhileRule(ASTNode* node, ASTNode* post) {
	
	ASTNode* condition = node->While.condition; // B
	ASTNode* invariant = node->While.invariant; // I (loop invariant)
	ASTNode* variant = node->While.variant;		// t (termination measure)
	DLL* block_code = node->While.block_main;	// S (loop body)

	// Create I ∧ B for the loop body precondition
	ASTNode* I_clone_1 = clone_node(invariant);
	ASTNode* condition_clone_1 = clone_node(condition);
	ASTNode* I_and_B = create_node_binary("and", I_clone_1, condition_clone_1);

	
	// Get weakest precondition for the loop body
	ASTNode* invariant_clone_for_wp = clone_node(invariant);
	ASTNode* wp_body = hoare_prover(block_code, I_and_B, invariant_clone_for_wp);
	
	
	// Free the consumed arguments
	free_ASTNode(I_and_B);
	free_ASTNode(invariant_clone_for_wp);


	// Build (I ∧ ¬B) -> post
	//   This encodes: when loop exits, invariant + ¬B imply the outer postcondition.
	ASTNode* I_clone_2 = clone_node(invariant);
	ASTNode* condition_clone_2 = clone_node(condition);
	ASTNode* not_condition = create_node_unary("not", condition_clone_2);
	ASTNode* I_and_notB = create_node_binary("and", I_clone_2, not_condition);


	// Create the implications for partial correctness
	ASTNode* post_clone = clone_node(post);
	ASTNode* right = create_node_binary("->", I_and_notB, post_clone);

	// Create I ∧ B again for the left side (since the previous one was consumed)
	// Build (I ∧ B) -> wp_body. i.e. {I ∧ B} S {I}.
	ASTNode* I_clone_3 = clone_node(invariant);
	ASTNode* condition_clone_3 = clone_node(condition);
	ASTNode* I_and_B_for_left = create_node_binary("and", I_clone_3, condition_clone_3);
	ASTNode* left = create_node_binary("->", I_and_B_for_left, wp_body);

	// Combine the two partial-correctness obligations: left ∧ right
	ASTNode* partial_correctness = create_node_binary("and", left, right);

	/*
		Termination checks (total correctness using a numeric variant)
		We compute 'variant_after' by simulating the effect of assignments in the loop body
		via substitution on the variant expression.
		That is: variant_after = variant[x := E] for assignments x := E in the body.
	*/
	ASTNode* variant_after = clone_node(variant);
	line_linkedlist* cur = block_code->first;
	while (cur != NULL) {
		if (cur->node->type == NODE_ASSIGN) {
			// Apply substitution variant_after[x := E]
			ASTNode* tmp = substitute(variant_after, cur->node->Assign.id, cur->node->Assign.expr);
			free_ASTNode(variant_after);
			variant_after = tmp;
		}
		cur = cur->next;
	}

	// Now variant_after is the expression for the variant after one body execution

	// variant_after < variant 
	ASTNode* variant_clone = clone_node(variant);
	ASTNode* variant_decreases = create_node_binary("<", variant_after, variant_clone);
 
	//  variant >= 0 (we assume a natural-number domain for the variant)
	ASTNode* variant_clone_2 = clone_node(variant);
	ASTNode* variant_nonnegative = create_node_binary(">=", variant_clone_2, create_node_number(0));

	// combine both decrease and non-negativity: (variant_after < variant) ∧ (variant >= 0)
	ASTNode* decrease_condition = create_node_binary("and", variant_decreases, variant_nonnegative);


	// Create (I ∧ B) -> decrease_condition
	ASTNode* I_clone_4 = clone_node(invariant);
	ASTNode* condition_clone_4 = clone_node(condition);
	ASTNode* I_and_B_for_term = create_node_binary("and", I_clone_4, condition_clone_4);
	ASTNode* termination_condition = create_node_binary("->", I_and_B_for_term, decrease_condition);


	//  partial_correctness ∧ termination_condition
	ASTNode* result = create_node_binary("and", partial_correctness, termination_condition);
	
	return result;
}

// Evaluates boolean formulas built from comparisons, logical ops and unary not.
// Returns 0 (false) or 1 (true).
int evaluate_formula(ASTNode* node) {
	if (!node) return 0;

	switch (node->type) {
		case NODE_BIN_OP : {
			// Logical connectors
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
				
			// Comparison operators: evaluate numeric subexpressions
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
			 // Unary logical NOT
			if( strcmp(node->unary_op.op,"NOT")==0 || strcmp(node->unary_op.op,"not")==0 ){
				int child = evaluate_formula(node->unary_op.child);
				return !child;
			}
			break;
		}

		default:
			// Unexpected node types are treated as false and reported
			fprintf(stderr, "evaluate_formula: unexpected node type %d\n", node->type);
			break;
	}

	return 0;
}

// Evaluates integer expressions (numbers, binary arithmetic, and simple functions).
// Returns an int result. Division is integer division.
int evaluate_expr (ASTNode* node) {
	if (!node) return 0;

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
			if(node->function.arg2 != NULL) arg2 = evaluate_expr(node->function.arg2);
			

			if( strcmp(node->function.fname,"min")==0 )		return (arg1 < arg2) ? arg1 : arg2;
			if( strcmp(node->function.fname,"max")==0 ) 	return (arg1 < arg2) ? arg2 : arg1;
			if ( strcmp(node->function.fname, "fact")==0 )	return fact(arg1);
			break;
		}

		default:
			// Unexpected node type
			fprintf(stderr, "evaluate_expr: unexpected node type %d\n", node->type);
			break;
	}
	return 0;
}
