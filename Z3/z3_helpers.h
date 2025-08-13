#ifndef Z3_HELPERS_H
#define Z3_HELPERS_H

#include <z3.h>
#include "ast.h"
#include "hashmap.h"

extern Z3_func_decl fact_func;

void init_z3(Z3_context ctx);
Z3_ast ast_to_z3(Z3_context ctx, ASTNode* node, HashMap* var_cache);
//int z3_check_equiv(ASTNode* a, ASTNode* b);

#endif