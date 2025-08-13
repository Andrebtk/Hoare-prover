#ifndef HOARE_H
#define HOARE_H

#include "ast.h"

#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define RESET   "\033[0m"
extern DLL* vc_list;
extern int eval_error; 
void substitute_vc_list(const char* var, ASTNode* replacement);


int is_node_true(ASTNode* node);

ASTNode* hoare_prover(DLL* code, ASTNode* pre, ASTNode* post);
ASTNode* hoare_statement(ASTNode* node, ASTNode* post);
ASTNode* hoare_AssignmentRule(ASTNode* node, ASTNode* post);
ASTNode* hoare_IfElseRule(ASTNode* node_IfElse, ASTNode* post);
ASTNode* hoare_WhileRule(ASTNode* node, ASTNode* post);

int evaluate_formula(ASTNode* node);
int evaluate_expr (ASTNode* node);

//bool is_tautology(ASTNode* node);

#endif