#ifndef HOARE_H
#define HOARE_H

#include "ast.h"

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"


int is_node_true(ASTNode* node);

ASTNode* hoare_prover(DLL* code, ASTNode* pre, ASTNode* post);
ASTNode* hoare_statement(ASTNode* node, ASTNode* post);
ASTNode* hoare_AssignmentRule(ASTNode* node, ASTNode* post);
ASTNode* hoare_IfElseRule(ASTNode* node_IfElse, ASTNode* post);
ASTNode* hoare_WhileRule(ASTNode* node, ASTNode* post);

int evaluate_formula(ASTNode* node);
int evaluate_expr (ASTNode* node);

#endif