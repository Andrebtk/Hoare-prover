#include "hoare.h"
#include <string.h>
#include "z3_helpers.h"
#include <stdlib.h>


int eval_error = 0;
// Global list to collect all Verification Conditions (VCs) from the program.
DLL* vc_list = NULL;

// A recursive function to compute factorial for integer constants.
int fact(int n) {
    if (n < 0) {
        // Factorial is not defined for negative numbers.
        // Return a sentinel value or handle as an error.
        fprintf(stderr, "Error: Factorial not defined for negative numbers\n");
        return 0; 
    }

    if (n == 0 || n == 1) {
        return 1;
    }

    return n * fact(n - 1);
}


// Checks if a node is the boolean literal `true`.
int is_node_true(ASTNode* node) {
    return (node->type == NODE_BOOL) && (node->bool_value == 1);
}

// Simple, recursive structural equality check for ASTNodes.
int ast_equal(const ASTNode* a, const ASTNode* b) {
    if (a == b) return 1;
    if (!a || !b) return 0;

    if (a->type != b->type) return 0;

    switch (a->type) {
        case NODE_NUMBER:
            return a->number == b->number;

        case NODE_ID:
            if (!a->id_name || !b->id_name) return a->id_name == b->id_name;
            return strcmp(a->id_name, b->id_name) == 0;

        case NODE_BOOL:
            return a->bool_value == b->bool_value;

        case NODE_UNARY_OP:
            if (!a->unary_op.op || !b->unary_op.op) return a->unary_op.op == b->unary_op.op;
            if (strcmp(a->unary_op.op, b->unary_op.op) != 0) return 0;
            return ast_equal(a->unary_op.child, b->unary_op.child);

        case NODE_BIN_OP:
            if (!a->binary_op.op || !b->binary_op.op) return a->binary_op.op == b->binary_op.op;
            if (strcmp(a->binary_op.op, b->binary_op.op) != 0) return 0;
            return ast_equal(a->binary_op.left, b->binary_op.left)
                && ast_equal(a->binary_op.right, b->binary_op.right);

        case NODE_FUNCTION:
            if (!a->function.fname || !b->function.fname) return a->function.fname == b->function.fname;
            if (strcmp(a->function.fname, b->function.fname) != 0) return 0;
            if (!ast_equal(a->function.arg1, b->function.arg1)) return 0;
            if (a->function.arg2 || b->function.arg2)
                return ast_equal(a->function.arg2, b->function.arg2);
            return 1;

        case NODE_ASSIGN:
            if (!a->Assign.id || !b->Assign.id) return a->Assign.id == b->Assign.id;
            if (strcmp(a->Assign.id, b->Assign.id) != 0) return 0;
            return ast_equal(a->Assign.expr, b->Assign.expr);

        case NODE_IF_ELSE:
            return ast_equal(a->If.condition, b->If.condition)
                && ((a->If.block_if == b->If.block_if) || (a->If.block_if && b->If.block_if))
                && ((a->If.block_else == b->If.block_else) || (a->If.block_else && b->If.block_else));

        case NODE_WHILE:
            return ast_equal(a->While.condition, b->While.condition)
                && ast_equal(a->While.invariant, b->While.invariant)
                && ast_equal(a->While.variant, b->While.variant)
                && ((a->While.block_main == b->While.block_main) || (a->While.block_main && b->While.block_main));

        default:
            return 0;
    }
}


/*
    Main function to compute the weakest precondition of a program.
    It performs a backward pass over the code and collects VCs from loops.
*/
ASTNode* hoare_prover(DLL* code, ASTNode* pre, ASTNode* post) {
    if (!code || !post) {
        fprintf(stderr, "NULL input to hoare_prover\n");
        return NULL;
    }

    // Initialize the global VC list.
    if (vc_list == NULL) {
        vc_list = create_DLL();
    }

    printf("=== HOARE PROVER ===\n");
    printf("Starting with post: ");
    print_AST_formula(post);
    printf("\n\n");

    line_linkedlist *current = code->last;
    ASTNode* wp = clone_node(post);

    while (current != NULL) {
        printf("Processing: ");
        if (current->node->type == NODE_ASSIGN) {
            printf("assignment %s = ", current->node->Assign.id);
            print_AST_formula(current->node->Assign.expr);
        } else if (current->node->type == NODE_WHILE) {
            printf("while loop");
        }
        printf("\n");
        
        printf("Current WP: ");
        print_AST_formula(wp);
        printf("\n");

        ASTNode* new_wp = hoare_statement(current->node, wp);
        if (new_wp == NULL) {
            fprintf(stderr, "hoare_prover: WP generation failed at a statement (aborting)\n");
            return NULL;
        }

        printf("New WP: ");
        print_AST_formula(new_wp);
        printf("\n\n");

        wp = new_wp;
        current = current->prec;
    }

    // Combine the final weakest precondition with all the collected VCs.
    ASTNode* final_formula = wp;
    line_linkedlist *vc_current = vc_list->first;
    while (vc_current != NULL) {
        final_formula = create_node_binary("and", final_formula, vc_current->node);
        vc_current = vc_current->next;
    }

    return final_formula;
}
/*
    Applies the correct Hoare rule based on the statement type.
    It is a wrapper around the specific rule functions.
*/
ASTNode* hoare_statement(ASTNode* node, ASTNode* post) {
    switch (node->type) {
        case NODE_ASSIGN : {
            return hoare_AssignmentRule(node, post);
        }
        case NODE_IF_ELSE: {
            return hoare_IfElseRule(node, post);
        }
        case NODE_WHILE: {
            return hoare_WhileRule(node, post);
        }
        default:
            fprintf(stderr, "hoare_statement: unsupported node type %d\n", node->type);
            return NULL;
    }
}




/*
    Hoare's Assignment Rule: wp(x := E, P) = P[x -> E]
*/
ASTNode* hoare_AssignmentRule(ASTNode* node, ASTNode* post) {
    ASTNode* result = substitute(post, node->Assign.id, node->Assign.expr);
    
    // CRITICAL: Also substitute in all collected VCs
    substitute_vc_list(node->Assign.id, node->Assign.expr);
    
    return result;
}

/*
    Hoare's If/Else Rule: wp(if B then C1 else C2, Q) = (B -> wp(C1, Q)) ∧ (¬B -> wp(C2, Q))
*/
ASTNode* hoare_IfElseRule(ASTNode* node_IfElse, ASTNode* post) {
    if (!node_IfElse || node_IfElse->type != NODE_IF_ELSE) {
        fprintf(stderr, "Invalid node in hoare_IfElseRule\n");
        return NULL;
    }

    
    ASTNode* condition = node_IfElse->If.condition; // B
    DLL* block_if = node_IfElse->If.block_if;       // C1
    DLL* block_else = node_IfElse->If.block_else;       // C2

    if (!condition || !block_if || !block_else) {
        fprintf(stderr, "Incomplete IF node\n");
        return NULL;
    }

    ASTNode* wp_if = hoare_prover(block_if, NULL, clone_node(post));
    ASTNode* wp_else = hoare_prover(block_else, NULL, clone_node(post));
    
    // The `hoare_prover` calls above will have populated the global vc_list.

    ASTNode* left = create_node_binary("->", condition, wp_if);
    ASTNode* n_right = create_node_unary("not", condition);
    ASTNode* right = create_node_binary("->", n_right, wp_else);

    return create_node_binary("and", left, right);
}


/*
    Hoare's While Rule: {I} while B do C {I ∧ ¬B}
    This function generates the VCs and returns the invariant `I`.
*/
/*
    Hoare's While Rule: {I} while B do C {I ∧ ¬B}
    This function generates the VCs and returns the invariant `I`.
*/
ASTNode* hoare_WhileRule(ASTNode* node, ASTNode* post) {
    if (!node || node->type != NODE_WHILE || !post) {
        fprintf(stderr, "hoare_WhileRule: invalid arguments\n");
        return NULL;
    }

    ASTNode* condition  = node->While.condition;   // B: z != x
    ASTNode* invariant  = node->While.invariant;   // I: y == fact(z)
    ASTNode* variant    = node->While.variant;     // V: x - z
    DLL* block_code = node->While.block_main;      // C: {z = z + 1; y = y * z;}

    if (!condition || !invariant || !variant || !block_code) {
        fprintf(stderr, "hoare_WhileRule: incomplete while node\n");
        return NULL;
    }
    
    // PARTIAL CORRECTNESS:
    // 1. The loop invariant must be preserved by the loop body: (I ∧ B) -> wp(C, I)
    ASTNode* I_and_B = create_node_binary("and", clone_node(invariant), clone_node(condition));
    ASTNode* wp_body = hoare_prover(block_code, I_and_B, clone_node(invariant));
    ASTNode* invariant_preservation = create_node_binary("->", I_and_B, wp_body);

    // 2. CRITICAL FIX: The postcondition must be derivable from invariant when loop exits
    // When loop exits: B is false, so ¬B is true, and we have I
    // We need: (I ∧ ¬B) -> post
    // BUT: we need to be careful about variable substitution here
    
    ASTNode* notB = create_node_unary("not", clone_node(condition));
    ASTNode* I_and_not_B = create_node_binary("and", clone_node(invariant), notB);
    ASTNode* postcondition_implies = create_node_binary("->", I_and_not_B, clone_node(post));

    // TOTAL CORRECTNESS (Termination):
    // 1. The variant must be a non-negative integer: (I ∧ B) -> variant >= 0
    ASTNode* variant_nonnegative_check = create_node_binary(
        "->", 
        clone_node(I_and_B), 
        create_node_binary(">=", clone_node(variant), create_node_number(0))
    );

    // 2. The variant must strictly decrease with each iteration: (I ∧ B) -> wp(C, variant) < variant
    ASTNode* wp_variant_body = hoare_prover(block_code, clone_node(I_and_B), clone_node(variant));
    ASTNode* variant_decreases_check = create_node_binary(
        "->", 
        clone_node(I_and_B), 
        create_node_binary("<", wp_variant_body, clone_node(variant))
    );
    
    // CRITICAL: Add VCs to list but DON'T substitute them yet
    // They should be substituted by the calling hoare_prover after this function returns
    DLL_append(vc_list, invariant_preservation);
    DLL_append(vc_list, postcondition_implies);
    DLL_append(vc_list, variant_nonnegative_check);
    DLL_append(vc_list, variant_decreases_check);

    // Return the invariant as the weakest precondition for the preceding statements.
    return clone_node(invariant);
}


void substitute_vc_list(const char* var, ASTNode* replacement) {
    if (!vc_list) return;
    
    printf("  Substituting %s = ", var);
    print_AST_formula(replacement);
    printf(" in all VCs...\n");
    
    line_linkedlist* current = vc_list->first;
    int vc_num = 1;
    while (current != NULL) {
        if (current->node) {
            printf("    VC%d before: ", vc_num);
            print_AST_formula(current->node);
            printf("\n");
            
            ASTNode* old_vc = current->node;
            ASTNode* new_vc = substitute(old_vc, var, replacement);
            current->node = new_vc;
            
            printf("    VC%d after:  ", vc_num);
            print_AST_formula(new_vc);
            printf("\n");
        }
        current = current->next;
        vc_num++;
    }
    printf("\n");
}



// 1) Logical & comparison evaluator
int evaluate_formula(ASTNode* node) {
    if (!node) return 0;
    eval_error = 0;
    switch (node->type) {
        case NODE_BIN_OP: {
            char* op = node->binary_op.op;
            if (strcmp(op, "AND") == 0 || strcmp(op, "and") == 0) {
                int L = evaluate_formula(node->binary_op.left);
                if (eval_error) return 0;
                if (!L) return 0;
                int R = evaluate_formula(node->binary_op.right);
                if (eval_error) return 0;
                return R;
            }

            if (strcmp(op, "OR") == 0 || strcmp(op, "or") == 0) {
                int L = evaluate_formula(node->binary_op.left);
                if (eval_error) return 0;
                if (L) return 1;
                int R = evaluate_formula(node->binary_op.right);
                if (eval_error) return 0;
                return R;
            }

            if (strcmp(op, "->") == 0) {
                int L = evaluate_formula(node->binary_op.left);
                if (eval_error) return 0;
                if (!L) return 1;
                int R = evaluate_formula(node->binary_op.right);
                if (eval_error) return 0;
                return (!L) || R;
            }

            int Lval = evaluate_expr(node->binary_op.left);
            if (eval_error) return 0;
            int Rval = evaluate_expr(node->binary_op.right);
            if (eval_error) return 0;

            if (strcmp(op, "==") == 0) return Lval == Rval;
            if (strcmp(op, "!=") == 0) return Lval != Rval;
            if (strcmp(op, "<") == 0) return Lval < Rval;
            if (strcmp(op, ">") == 0) return Lval > Rval;
            if (strcmp(op, "<=") == 0) return Lval <= Rval;
            if (strcmp(op, ">=") == 0) return Lval >= Rval;

            fprintf(stderr, "evaluate_formula: unknown binary operator '%s'\n", op);
            return 0;
        }

        case NODE_UNARY_OP: {
            if (!node->unary_op.op) { eval_error = 1; return 0; }
            if (strcmp(node->unary_op.op, "NOT") == 0 || strcmp(node->unary_op.op, "not") == 0) {
                int c = evaluate_formula(node->unary_op.child);
                if (eval_error) return 0;
                return !c;
            }
            fprintf(stderr, "evaluate_formula: unknown unary op '%s'\n", node->unary_op.op);
            return 0;
        }

        case NODE_BOOL:
            return node->bool_value ? 1 : 0;

        case NODE_NUMBER:
            return (node->number != 0);

        case NODE_ID:
            if (node->id_name) fprintf(stderr, "evaluate_formula: unresolved identifier '%s' in formula\n", node->id_name);
            else fprintf(stderr, "evaluate_formula: unresolved identifier (NULL) in formula\n");
            eval_error = 1;
            return 0;

        default:
            fprintf(stderr, "evaluate_formula: unexpected node type %d\n", node->type);
            eval_error = 1;
            return 0;
    }
}

// 2) Arithmetic evaluator
int evaluate_expr(ASTNode* node) {
    if (!node) {
        eval_error = 1;
        return 0;
    }

    switch (node->type) {
        case NODE_NUMBER:
            return node->number;

        case NODE_ID:
            fprintf(stderr, "evaluate_expr: unresolved identifier '%s'\n", node->id_name ? node->id_name : "(NULL)");
            eval_error = 1;
            return 0;

        case NODE_BIN_OP: {
            int L = evaluate_expr(node->binary_op.left);
            if (eval_error) return 0;
            int R = evaluate_expr(node->binary_op.right);
            if (eval_error) return 0;

            char* op = node->binary_op.op;
            if (strcmp(op, "+") == 0) return L + R;
            if (strcmp(op, "-") == 0) return L - R;
            if (strcmp(op, "*") == 0) return L * R;
            if (strcmp(op, "/") == 0) {
                if (R == 0) { eval_error = 1; fprintf(stderr, "Division by zero\n"); return 0; }
                return L / R;
            }
            fprintf(stderr, "Unsupported arithmetic op '%s'\n", op);
            eval_error = 1;
            return 0;
        }

        case NODE_FUNCTION: {
            if (!node->function.fname) { eval_error = 1; return 0; }
            if (strcmp(node->function.fname, "fact") == 0) {
                int arg = evaluate_expr(node->function.arg1);
                if (eval_error) return 0;
                return fact(arg);
            }
            if (strcmp(node->function.fname, "min") == 0 || strcmp(node->function.fname, "max") == 0) {
                int a = evaluate_expr(node->function.arg1);
                int b = evaluate_expr(node->function.arg2);
                if (eval_error) return 0;
                return strcmp(node->function.fname, "min") == 0 ? (a < b ? a : b) : (a > b ? a : b);
            }
            fprintf(stderr, "Unknown function '%s'\n", node->function.fname);
            eval_error = 1;
            return 0;
        }

        default:
            fprintf(stderr, "evaluate_expr: unexpected node type %d\n", node->type);
            eval_error = 1;
            return 0;
    }
}


// Add this debug function to your main parser file
void debug_substitution_step(const char* var, ASTNode* value, ASTNode* before, ASTNode* after) {
    printf("=== SUBSTITUTION STEP ===\n");
    printf("Variable: %s = ", var);
    print_AST_formula(value);
    printf("\n");
    printf("Before: ");
    print_AST_formula(before);
    printf("\n");
    printf("After:  ");
    print_AST_formula(after);
    printf("\n\n");
}

// Modified hoare_prover with debugging
ASTNode* hoare_prover_debug(DLL* code, ASTNode* pre, ASTNode* post) {
    if (!code || !post) {
        fprintf(stderr, "NULL input to hoare_prover\n");
        return NULL;
    }

    // Initialize the global VC list if not already done
    if (vc_list == NULL) {
        vc_list = create_DLL();
    }

    printf("\n=== STARTING BACKWARD PASS ===\n");
    printf("Initial postcondition: ");
    print_AST_formula(post);
    printf("\n\n");

    line_linkedlist *current = code->last;
    ASTNode* wp = clone_node(post);
    int step = 1;

    // Backward pass through all statements
    while (current != NULL) {
        printf("--- STEP %d ---\n", step++);
        
        if (current->node->type == NODE_ASSIGN) {
            printf("Processing assignment: %s = ", current->node->Assign.id);
            print_AST_formula(current->node->Assign.expr);
            printf("\n");
        } else if (current->node->type == NODE_WHILE) {
            printf("Processing while loop\n");
        }
        
        printf("Current WP: ");
        print_AST_formula(wp);
        printf("\n");
        
        ASTNode* new_wp = hoare_statement(current->node, wp);
        if (new_wp == NULL) {
            fprintf(stderr, "hoare_prover: WP generation failed at a statement (aborting)\n");
            return NULL;
        }

        if (current->node->type == NODE_ASSIGN) {
            debug_substitution_step(current->node->Assign.id, current->node->Assign.expr, wp, new_wp);
        }

        wp = new_wp;
        current = current->prec;
    }

    printf("=== FINAL WP BEFORE VCs ===\n");
    print_AST_formula(wp);
    printf("\n\n");

    // Combine the final weakest precondition with all the collected VCs.
    ASTNode* final_formula = wp;
    line_linkedlist *vc_current = vc_list->first;
    int vc_count = 1;
    
    while (vc_current != NULL) {
        printf("Adding VC %d: ", vc_count++);
        print_AST_formula(vc_current->node);
        printf("\n");
        
        final_formula = create_node_binary("and", final_formula, vc_current->node);
        vc_current = vc_current->next;
    }

    printf("\n=== FINAL VERIFICATION CONDITION ===\n");
    print_AST_formula(final_formula);
    printf("\n");

    return final_formula;
}


// Test evaluation step by step
int evaluate_and_trace(ASTNode* node) {
    if (!node) {
        printf("Cannot evaluate NULL node\n");
        return 0;
    }
    
    printf("Evaluating: ");
    print_AST_formula(node);
    printf("\n");
    
    eval_error = 0;
    int result = evaluate_formula(node);
    
    if (eval_error) {
        printf("  -> ERROR: Evaluation failed (unresolved variables)\n");
        return 0;
    }
    
    printf("  -> Result: %s\n", result ? "TRUE" : "FALSE");
    return result;
}
