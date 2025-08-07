%{ 
	/* C declarations: includes, helper functions, variables */
	#include <stdio.h>
	#include <stdlib.h>
	#include "data-struct.h"

	DLL* root = NULL;

	void yyerror(const char *s);
	int yylex(void);

	//int yydebug = 1;

%}

/* Declare tokens here */
%union {
	int num;
	char *id;
	ASTNode* node;
	DLL* dll;
}


%token IF ELSE WHILE 
%token SEMICOLON LPAREN RPAREN LBRACE RBRACE COLON
%token PLUS MINUS MUL DIV LT GT AND OR
%token PRECOND POSTCOND 

%token TRUE FALSE

%token MIN MAX
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

 	IDENTIFIER ASSIGN expr SEMICOLON					{ 
		$$ = create_node_assign($1, $3);
	}

	| IF LPAREN condition RPAREN block ELSE block		{ 
		$$ = create_node_If_Else($3, $5, $7);
	}

	| IF LPAREN condition RPAREN block {
		$$ = create_node_If_Else($3, $5, NULL); // Handle if without else
	}

	| WHILE LPAREN condition RPAREN block 				{ 
		$$ = create_node_While($3, $5);
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
    | expr EQ expr          { $$ = create_node_binary("==", $1, $3); }
    | expr NEQ expr         { $$ = create_node_binary("!=", $1, $3); }
	| TRUE                  { $$ = create_node_bool(1); }
    | FALSE                 { $$ = create_node_bool(0); }
    | condition AND condition { $$ = create_node_binary("and", $1, $3); }
    | condition OR condition  { $$ = create_node_binary("or", $1, $3); }
    | NOT condition         { $$ = create_node_unary("not", $2); }
    | LPAREN condition RPAREN { $$ = $2; }
    | condition IMPLY condition { $$ = create_node_binary("->", $1, $3); }
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
    ;

%%

/* Additional C code (functions, main, helpers) */
void yyerror(const char *s) {
	fprintf(stderr, "Parse error: %s\n", s);
}

int main() {

	printf("Start parsing...\n");
    
	
	//yydebug = 1;
	if (yyparse() == 0) {
        printf("Parsing done.\n");
        // root now points to your DLL with all statements

		print_DLL(root, 0, 0);
    } else {
        printf("Parsing failed.\n");
		return -1;
    }

	printf("Starting verify:\n");
	
	ASTNode* result = hoare_prover(root, root->pre, root->post);
	
	
	printf("\nResult: ");
	if (evaluate_formula(result) == 0)	{ printf(RED "The program is not valid\n" RESET); }
	else 								{ printf(GREEN "The program is valid\n" RESET); }
	
	return 0;

}