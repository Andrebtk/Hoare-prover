%{ 
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
	extern int yylex_destroy(void);
	//int yydebug = 1;

%}

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
%token MOD
%token TRUE FALSE
%token MIN MAX FACT
%token COMMA
%token IMPLY
%token ASSIGN
%token EQ
%token NEQ
%token NOT

%token <id> IDENTIFIER 
%token <num> NUMBER 
%type <node> statement expr condition precond postcond
%type <dll> block statements

%right IMPLY
%left OR
%left AND
%right NOT
%nonassoc LT GT EQ NEQ
%left PLUS MINUS
%left MUL DIV MOD
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
 	IDENTIFIER ASSIGN expr SEMICOLON { 
		$$ = create_node_assign($1, $3);
		free($1);
	}

	| IF LPAREN condition RPAREN block ELSE block { 
		$$ = create_node_If_Else($3, $5, $7);
	}

	| IF LPAREN condition RPAREN block {
		$$ = create_node_If_Else($3, $5, NULL);
	}

	| WHILE LPAREN condition RPAREN INVARIANT LPAREN condition RPAREN VARIANT LPAREN expr RPAREN block 	{ 
		$$ = create_node_While($3, $13, $7, $11);
	}
;

statements:
						{
		// Base case: an empty "statements" rule
		// -> when there are no statements, create a new (empty) DLL to hold them
		$$ = create_DLL(); 
    }
	| statements statement		{ 
		// Recursive case: we already have a list of statements ($1)
		// Append the new "statement" ($2) to that list
		DLL_append($1, $2); 
		$$ = $1;
	}
;

block:
	LBRACE statements RBRACE { 
		$$ = $2; 
	}
;

condition:
	  expr LT expr					{ $$ = create_node_binary("<", $1, $3); }
	| expr GT expr					{ $$ = create_node_binary(">", $1, $3); }
	| expr GE expr					{ $$ = create_node_binary(">=", $1, $3); }
	| expr LE expr					{ $$ = create_node_binary("<=", $1, $3); }
	| expr EQ expr					{ $$ = create_node_binary("==", $1, $3); }
	| expr NEQ expr					{ $$ = create_node_binary("!=", $1, $3); }
	| TRUE							{ $$ = create_node_bool(1); }
	| FALSE							{ $$ = create_node_bool(0); }
	| condition AND condition		{ $$ = create_node_binary("and", $1, $3); }
	| condition OR condition		{ $$ = create_node_binary("or", $1, $3); }
	| NOT condition					{ $$ = create_node_unary("not", $2); }
	| LPAREN condition RPAREN		{ $$ = $2; }
	| condition IMPLY condition		{ $$ = create_node_binary("->", $1, $3); }
	| expr							{ $$ = $1; }
;

expr:
	  NUMBER								{ $$ = create_node_number($1); }
	| IDENTIFIER							{ $$ = create_node_id($1); free($1);}
	| expr PLUS expr						{ $$ = create_node_binary("+", $1, $3); }
	| expr MINUS expr						{ $$ = create_node_binary("-", $1, $3); }
	| expr MUL expr							{ $$ = create_node_binary("*", $1, $3); }
	| expr DIV expr							{ $$ = create_node_binary("/", $1, $3); }
	| expr MOD expr							{ $$ = create_node_binary("%", $1, $3); }
	| LPAREN expr RPAREN 					{ $$ = $2; }
	| MIN LPAREN expr COMMA expr RPAREN		{ $$ = create_node_Func("min", $3, $5); }
	| MAX LPAREN expr COMMA expr RPAREN		{ $$ = create_node_Func("max", $3, $5); }
	| FACT LPAREN expr RPAREN				{ $$ = create_node_Func("fact", $3, NULL); }
;

%%
/* Additional C code (functions, main, helpers) */

void yyerror(const char *s) {
	fprintf(stderr, "Parse error: %s\n", s);
}

Z3_func_decl fact_func;



int main() {
	printf("Start parsing...\n");


	// ----------------------------
	// Parse input program
	// ----------------------------
	if (yyparse() == 0) {
		printf("Parsing done.\n"); // root now points to your DLL with all statements
	} else {
		printf("Parsing failed.\n");
		return -1;
	}

	// Free lexer state after parsing
	yylex_destroy();

	
	printf("Starting verification...\n");

	
	// ----------------------------
	// Sanity check: reject pre = true (unsupported)
	// ----------------------------
	if (is_node_true(root->pre)) {
		printf(RED "ERROR -> \"PRECONDITION: true\" is not supported\n" RESET);
		free_DLL(root);  // cleanup AST
		return 1;
	}

	// ----------------------------
	// Generate verification condition (VC) from program
	// ----------------------------
	ASTNode* result = hoare_prover(root, root->pre, root->post);
	ASTNode* vc = create_node_binary("->", clone_node(root->pre), clone_node(result));

	// ----------------------------
	// Setup Z3 solver
	// ----------------------------
	Z3_config cfg = Z3_mk_config();
	Z3_context ctx = Z3_mk_context(cfg);
	Z3_del_config(cfg);

	init_z3(ctx); // user-defined funcs like fact

	Z3_solver solver = Z3_mk_solver(ctx);
	Z3_solver_inc_ref(ctx, solver);


	// Cache for variables (so we reuse Z3 symbols consistently)
	HashMap* var_cache = create_HashMap(16);


	// ----------------------------
	// Translate VC into Z3 formula
	// ----------------------------
	Z3_ast res = ast_to_z3(ctx, vc, var_cache);
	Z3_inc_ref(ctx, res);

	// Build ¬VC (negation of VC)
	Z3_ast not_vc = Z3_mk_not(ctx, res);
	Z3_inc_ref(ctx, not_vc);

	// Assert ¬VC (to check validity)
	
	Z3_solver_assert(ctx, solver, not_vc); // add constraint not_vc to solver

	// ----------------------------
	// Check satisfiability
	// ----------------------------
	Z3_lbool z3_result = Z3_solver_check(ctx, solver); //Run the solver
	
	if (z3_result == Z3_L_FALSE) {
		// UNSAT(¬VC) ⇒ VC is valid
		printf(GREEN "Z3 says: The program is correct!\n" RESET);
	} 
	else if (z3_result == Z3_L_TRUE) {
		 // SAT(¬VC) ⇒ counterexample exists
		printf(RED "Z3 says: The program is NOT correct!\n" RESET);
	} 
	else {
		printf("Z3 says: Unknown result.\n");
	}

	// ----------------------------
	// Cleanup
	// ----------------------------
	Z3_solver_dec_ref(ctx, solver);
	Z3_dec_ref(ctx, res);
	Z3_dec_ref(ctx, not_vc);
	free_hashmap_with_context(var_cache, ctx);
	Z3_del_context(ctx);

	// Free ASTs
	free_ASTNode(vc);
	free_ASTNode(result);
	free_DLL(root);

	Z3_finalize_memory();

	return 0;
}
