%{ 
	/* C declarations: includes, helper functions, variables */
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "../Ast/ast.h"
	#include "../Hoare/hoare.h"
	#include "../Z3/z3_helpers.h"
	#include "../Hashmap/hashmap.h"
	#include "../Hoare/hoare.h"

	DLL* root = NULL;

	void yyerror(const char *s);
	int yylex(void);

	//int yydebug = 1;
	int verify_program_concrete(DLL* program);
%}

/* Declare tokens here */
%union {
	int num;
	char *id;
	ASTNode* node;
	DLL* dll;
}


%token IF ELSE WHILE INVARIANT VARIANT
%token SEMICOLON LPAREN RPAREN LBRACE RBRACE COLON
%token PLUS MINUS MUL DIV LT GT AND OR
%token GE LE
%token PRECOND POSTCOND 

%token TRUE FALSE

%token MIN MAX FACT
%token COMMA
%token IMPLY

%token <id> IDENTIFIER 
%token <num> NUMBER 

%type <node> statement expr condition precond postcond
%type <dll> block statements

%token ASSIGN    // '='
%token EQ        // '=='
%token NEQ       // '!='
%token NOT

%right IMPLY
%left OR
%left AND
%right NOT
%nonassoc LT GT EQ NEQ
%left PLUS MINUS
%left MUL DIV
%right ASSIGN




%%
/* Grammar rules and actions */
program:
	statements precond postcond {
		
		root = $1;
		root->pre = $2;
		root->post = $3;
	}
	;

precond: 
	PRECOND condition {
		$$ = $2;
		
	}
	;

postcond:
	POSTCOND condition {
		$$ = $2;
	}
	;


statement:

 	IDENTIFIER ASSIGN expr SEMICOLON											{ 
		$$ = create_node_assign($1, $3);
	}

	| IF LPAREN condition RPAREN block ELSE block								{ 
		$$ = create_node_If_Else($3, $5, $7);
	}

	| IF LPAREN condition RPAREN block 											{
		$$ = create_node_If_Else($3, $5, NULL); // Handle if without else
	}

	| WHILE LPAREN condition RPAREN INVARIANT LPAREN condition RPAREN VARIANT LPAREN expr RPAREN block 	{ 
		$$ = create_node_While($3, $13, $7, $11);
	}

	
	
	;

statements:
	  							{
		$$ = create_DLL(); 
	  }
	| statements statement		{ 
		DLL_append($1, $2); 
		$$ = $1;
	}
	;

block:
	LBRACE statements RBRACE { $$ = $2; }
	;

condition:
      expr LT expr          { $$ = create_node_binary("<", $1, $3); }
    | expr GT expr          { $$ = create_node_binary(">", $1, $3); }
	| expr GE expr          { $$ = create_node_binary(">=", $1, $3); }
	| expr LE expr          { $$ = create_node_binary("<=", $1, $3); }
    | expr EQ expr          { $$ = create_node_binary("==", $1, $3); }
    | expr NEQ expr         { $$ = create_node_binary("!=", $1, $3); }
	| TRUE                  { $$ = create_node_bool(1); }
    | FALSE                 { $$ = create_node_bool(0); }
    | condition AND condition { $$ = create_node_binary("and", $1, $3); }
    | condition OR condition  { $$ = create_node_binary("or", $1, $3); }
    | NOT condition         { $$ = create_node_unary("not", $2); }
    | LPAREN condition RPAREN { $$ = $2; }
    | condition IMPLY condition { $$ = create_node_binary("->", $1, $3); }
	| expr                  { $$ = $1; }
    ;

expr:
      NUMBER                { $$ = create_node_number($1); }
    | IDENTIFIER            { $$ = create_node_id($1); }
    | expr PLUS expr        { $$ = create_node_binary("+", $1, $3); }
    | expr MINUS expr       { $$ = create_node_binary("-", $1, $3); }
    | expr MUL expr         { $$ = create_node_binary("*", $1, $3); }
    | expr DIV expr         { $$ = create_node_binary("/", $1, $3); }
    | LPAREN expr RPAREN    { $$ = $2; }
    | MIN LPAREN expr COMMA expr RPAREN {
        $$ = create_node_Func("min", $3, $5);
    }
    | MAX LPAREN expr COMMA expr RPAREN {
        $$ = create_node_Func("max", $3, $5);
    }
	| FACT LPAREN expr RPAREN {
		$$ = create_node_Func("fact", $3, NULL);
	}
    ;

%%

/* Additional C code (functions, main, helpers) */
void yyerror(const char *s) {
	fprintf(stderr, "Parse error: %s\n", s);
}


/*
Z3_func_decl fact_func;
void init_z3_functions(Z3_context ctx) {
    Z3_sort int_sort = Z3_mk_int_sort(ctx);

    Z3_symbol fact_sym = Z3_mk_string_symbol(ctx, "fact");

    // La fonction fact prend 1 argument de type int, retourne int
    Z3_sort domain[] = { int_sort };

    fact_func = Z3_mk_func_decl(ctx, fact_sym, 1, domain, int_sort);
}
*/
int print_and_eval_vc(ASTNode* vc) {
    if (!vc) return 0;
    printf("---- Verification condition (AST) ----\n");
    print_AST_formula(vc);
    printf("\n");

    if (vc->type == NODE_BIN_OP && strcmp(vc->binary_op.op, "and") == 0) {
        printf("Left conjunct:\n"); print_AST_formula(vc->binary_op.left); printf("\n");
        printf("Eval left -> %d\n", evaluate_formula(vc->binary_op.left));
        printf("Right conjunct:\n"); print_AST_formula(vc->binary_op.right); printf("\n");
        printf("Eval right -> %d\n", evaluate_formula(vc->binary_op.right));
        return evaluate_formula(vc->binary_op.left) && evaluate_formula(vc->binary_op.right);
    } else {
        int v = evaluate_formula(vc);
        printf("VC eval -> %d\n", v);
        return v;
    }
}

int main() {

	printf("Start parsing...\n");
    
    if (yyparse() == 0) {
        printf("Parsing done.\n");
    } else {
        printf("Parsing failed.\n");
        return -1;
    }

    // Use the debug version instead
    verify_program_concrete(root);

	/*
	printf("Start parsing...\n");
    
	
	//yydebug = 1;
	if (yyparse() == 0) {
        printf("Parsing done.\n");
        // root now points to your DLL with all statements

		//print_DLL(root, 0, 0);
    } else {
        printf("Parsing failed.\n");
		return -1;
    }

	printf("Starting verify:\n");
	
	ASTNode* result = hoare_prover(root, root->pre, root->post);
	print_and_eval_vc(result);


	printf("\nResult: ");
	if (evaluate_formula(result) == 0)	{ printf(RED "The program is not valid\n" RESET); }
	else 								{ printf(GREEN "The program is valid\n" RESET); }
	*/



	/*
	printf("Start parsing...\n");
	
	if (yyparse() == 0) { printf("Parsing done.\n"); } 
	else { printf("Parsing failed.\n"); return -1; }
	
	printf("Starting verify:\n");
	
	ASTNode* result = hoare_prover(root, root->pre, root->post);
	print_ASTNode(result, -1, 0);
	
	if (result == NULL) {
		fprintf(stderr, RED "Verification aborted: weakest precondition generation failed.\n" RESET);
		return -1;
	}
	
	ASTNode* vc = create_node_binary("->", root->pre, result);
	if (!vc) {
		fprintf(stderr, RED "Failed to construct verification condition (vc is NULL)\n" RESET);
		return -1;
	}


	if( !is_node_true(root->pre) ) {
		printf(RED "Current version does only support -> \"PRECONDITION: true\"\n" RESET);
		return 0;
	}

	if ( evaluate_formula(result) == 1 )	{ 
		printf(GREEN "The program is valid\n" RESET);
		return 0;
	} 
	else {
		printf(RED "The program is not valid\n" RESET);
	}
	*/


	
	/*
	// --------- Here comes Z3 verification ---------
	// Create context and solver
	
	Z3_config cfg = Z3_mk_config();
	Z3_context ctx = Z3_mk_context(cfg);
	Z3_del_config(cfg);

	
	init_z3_functions(ctx);
	init_z3(ctx);
	

	Z3_solver solver = Z3_mk_solver(ctx);
	Z3_solver_inc_ref(ctx, solver);

	// Create var_cache hashmap for variable caching
	HashMap* var_cache = create_HashMap(64);

	
	// Convert VC AST (result) to Z3_ast
	Z3_ast res = ast_to_z3(ctx, vc, var_cache);
	if (res == NULL) {
		fprintf(stderr, RED "ast_to_z3 returned NULL (cannot check VC)\n" RESET);
		return -1;
	}
	
	// Assert negation of VC to check validity
	Z3_ast not_vc = Z3_mk_not(ctx, res);
	Z3_solver_assert(ctx, solver, not_vc);
	
	// Check satisfiability
	Z3_lbool z3_result = Z3_solver_check(ctx, solver);
	if (z3_result == Z3_L_FALSE) {
		printf(GREEN "Z3 says: The program is correct!\n" RESET);
	} 
	else if (z3_result == Z3_L_TRUE) {
		printf(RED "Z3 says: The program is NOT correct!\n" RESET);
		Z3_model model = Z3_solver_get_model(ctx, solver);
		Z3_model_inc_ref(ctx, model);

		Z3_model_dec_ref(ctx, model);
	} else {
		printf("Z3 says: Unknown result.\n");
	}
	
	// Cleanup
	Z3_solver_dec_ref(ctx, solver);
	free_hashmap_with_context(var_cache, ctx);
	Z3_del_context(ctx);
	
	*/
	free_DLL(root);
	return 0;
	
}

int verify_program_concrete(DLL* program) {
    printf("\n========== CONCRETE VERIFICATION ==========\n");
    
    // Clear any existing VCs
    if (vc_list) {
        // Free the old list
        line_linkedlist* current = vc_list->first;
        while (current) {
            line_linkedlist* next = current->next;
            // Don't free the nodes - they might be shared
            free(current);
            current = next;
        }
        free(vc_list);
        vc_list = NULL;
    }
    
    printf("Initial postcondition: ");
    print_AST_formula(program->post);
    printf("\n\n");
    
    ASTNode* final_vc = hoare_prover(program, program->pre, program->post);
    
    if (!final_vc) {
        printf("FAILED: Could not generate verification condition\n");
        return 0;
    }
    
    printf("=== FINAL VERIFICATION CONDITION ===\n");
    print_AST_formula(final_vc);
    printf("\n\n");
    
    // Try to evaluate
    eval_error = 0;
    int result = evaluate_formula(final_vc);
    
    if (eval_error) {
        printf("ERROR: Could not evaluate - still contains unresolved variables\n");
        return 0;
    }
    
    printf("Evaluation result: %s\n", result ? "TRUE (VALID)" : "FALSE (INVALID)");
    return result;
}