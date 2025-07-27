%{ 
	/* C declarations: includes, helper functions, variables */
	#include <stdio.h>
	#include <stdlib.h>
	#include "data-struct.h"

	DLL* root = NULL;

	void yyerror(const char *s);
	int yylex(void);

%}

/* Declare tokens here */
%union {
	int num;
	char *id;
	ASTNode* node;
	DLL* dll;
}


%token IF WHILE 
%token SEMICOLON LPAREN RPAREN LBRACE RBRACE
%token PLUS MINUS MUL DIV LT GT


%token <id> IDENTIFIER 
%token <num> NUMBER 

%type <node> statement expr condition
%type <dll> block statements

%token ASSIGN    // '='
%token EQ        // '=='
%token NEQ       // '!='


%left PLUS MINUS
%left MUL DIV
%left LT GT


%%
/* Grammar rules and actions */
program:
	statements {
		root = $1;
	}
	;

statement:
	  IDENTIFIER ASSIGN expr SEMICOLON			{ 
		$$ = create_node_assign($1, $3);
	}

	| IF LPAREN condition RPAREN block 			{ 
		$$ = create_node_If($3, $5);
	}

	| WHILE LPAREN condition RPAREN block 		{ 
		$$ = create_node_While($3, $5);
	}
	;

statements:
								{ $$ = create_DLL(); }
	| statements statement		{ 
		DLL_append($1, $2); 
		$$ = $1;
	}
	;

block:
	LBRACE statements RBRACE { $$ = $2; }
	;

condition:
	  expr LT expr 			{ $$ = create_node_binary("<", $1, $3); }
	| expr GT expr			{ $$ = create_node_binary(">", $1, $3); }
	| expr EQ expr			{ $$ = create_node_binary("==", $1, $3); }
	| expr NEQ expr			{ $$ = create_node_binary("!=", $1, $3); }
	;

expr:
	  NUMBER				{ $$ = create_node_number($1); }
	| IDENTIFIER			{ $$ = create_node_id($1); }
	| expr PLUS expr		{ $$ = create_node_binary("+", $1, $3); }
	| expr MINUS expr		{ $$ = create_node_binary("-", $1, $3); }
	| expr MUL expr			{ $$ = create_node_binary("*", $1, $3); }
	| expr DIV expr			{ $$ = create_node_binary("/", $1, $3); }
	| expr EQ expr			{ $$ = create_node_binary("==", $1, $3); }
	| expr NEQ expr			{ $$ = create_node_binary("!=", $1, $3); }
	;

%%

/* Additional C code (functions, main, helpers) */
void yyerror(const char *s) {
	fprintf(stderr, "Parse error: %s\n", s);
}

int main() {

	printf("Start parsing...\n");
    if (yyparse() == 0) {
        printf("Parsing done.\n");
        // root now points to your DLL with all statements

		print_DLL(root);
    } else {
        printf("Parsing failed.\n");
    }

    return 0;

}