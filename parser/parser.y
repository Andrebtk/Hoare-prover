%{ 
	/* C declarations: includes, helper functions, variables */
	#include <stdio.h>
	#include <stdlib.h>

	void yyerror(const char *s);
	int yylex(void);

%}

/* Declare tokens here */

%token IF WHILE 
%token SEMICOLON LPAREN RPAREN LBRACE RBRACE
%token PLUS MINUS MUL DIV LT GT


%token <id> IDENTIFIER 
%token <num> NUMBER 

%token ASSIGN    // '='
%token EQ        // '=='
%token NEQ       // '!='


%left PLUS MINUS
%left MUL DIV
%left LT GT

%union {
	int num;
	char *id;
}

%%
/* Grammar rules and actions */
program:
	statements
	;

statement:
	  IDENTIFIER ASSIGN expr SEMICOLON			{ 
		printf("Assignment\n"); 
	}

	| IF LPAREN condition RPAREN block 			{ 
		printf("If statement\n"); 
	}

	| WHILE LPAREN condition RPAREN block 		{ 
		printf("while loop\n"); 
	}
	;

statements:
	| statements statement
	;

block:
	LBRACE statements RBRACE
	;

condition:
	  expr LT expr
	| expr GT expr
	| expr EQ expr
	| expr NEQ expr
	;

expr:
	  NUMBER
	| IDENTIFIER
	| expr PLUS expr
	| expr MINUS expr
	| expr MUL expr
	| expr DIV expr
	| expr EQ expr
	| expr NEQ expr
	;

%%

/* Additional C code (functions, main, helpers) */
void yyerror(const char *s) {
	fprintf(stderr, "Parse error: %s\n", s);
}

int main() {
	printf("Start parsing...\n");
	yyparse();
	return 0;
}