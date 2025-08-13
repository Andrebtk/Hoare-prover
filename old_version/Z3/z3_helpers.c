#include "z3_helpers.h"
#include <string.h>


Z3_func_decl fact_func = NULL;

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
        }

        case NODE_BOOL: {
            return (node->bool_value) ? Z3_mk_true(ctx) : Z3_mk_false(ctx);
        }

        case NODE_ID: {
            if (!var_cache) {
                fprintf(stderr, "Error: var_cache is NULL\n");
                return NULL;
            }
            if (!node->id_name) {
                fprintf(stderr, "ast_to_z3: NULL id_name in NODE_ID\n");
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

            // Not found, so create a new Z3 variable and insert it into the cache
            Z3_symbol sym = Z3_mk_string_symbol(ctx, node->id_name);
            Z3_ast var = Z3_mk_const(ctx, sym, int_sort);
            insert_HashMap(var_cache, node->id_name, var);
            return var;
        }

        case NODE_BIN_OP: {
            if (!node->binary_op.op) {
                fprintf(stderr, "ast_to_z3: NULL op in NODE_BIN_OP\n");
                return NULL;
            }
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
            if (strcmp(node->binary_op.op, ">=") == 0) { return Z3_mk_ge(ctx, left, right); }
            if (strcmp(node->binary_op.op, "<=") == 0) { return Z3_mk_le(ctx, left, right); }
            if (strcmp(node->binary_op.op, "->") == 0) { return Z3_mk_implies(ctx, left, right); }

            fprintf(stderr, "ast_to_z3: Unknown binary op '%s'\n", node->binary_op.op);
            return NULL;
        }

        case NODE_UNARY_OP: {
            if (!node->unary_op.op) {
                fprintf(stderr, "ast_to_z3: NULL op in NODE_UNARY_OP\n");
                return NULL;
            }
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
            if (!node->function.fname) {
                fprintf(stderr, "ast_to_z3: NULL fname in NODE_FUNCTION\n");
                return NULL;
            }
            if (strcmp(node->function.fname, "fact") == 0) {
                extern Z3_func_decl fact_func; 
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

/*
int z3_check_equiv(ASTNode* a, ASTNode* b) {
    Z3_config cfg = Z3_mk_config();
    Z3_context ctx = Z3_mk_context(cfg);
    Z3_solver solver = Z3_mk_solver(ctx);
    Z3_solver_inc_ref(ctx, solver);

    HashMap* var_cache = create_HashMap(64);

    Z3_ast za = ast_to_z3(ctx, a, var_cache);
    Z3_ast zb = ast_to_z3(ctx, b, var_cache);

    printf("Expected: %s\n", Z3_ast_to_string(ctx, za));
    printf("Actual:   %s\n", Z3_ast_to_string(ctx, zb));

    if (!za || !zb) {
        fprintf(stderr, "Z3 conversion failed\n");
        Z3_solver_dec_ref(ctx, solver);
        Z3_del_context(ctx);
        free_hashmap(var_cache);
        return 0;
    }

    // Check ¬(a ⇔ b)
    Z3_ast equiv = Z3_mk_not(ctx, Z3_mk_eq(ctx, za, zb));
    Z3_solver_assert(ctx, solver, equiv);

    Z3_lbool result = Z3_solver_check(ctx, solver);

    Z3_solver_dec_ref(ctx, solver);
    Z3_del_context(ctx);
    free_hashmap(var_cache);

    return result == Z3_L_FALSE; // unsat means ¬(a ⇔ b) is false, so a and b are equivalent
}
*/