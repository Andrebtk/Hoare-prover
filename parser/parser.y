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

%type <num> expr condition
%type <num> block statements

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
		printf("Assignment %s = %d\n", $1, $3); 
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
	LBRACE statements RBRACE {  }
	;

condition:
	  expr LT expr 			{ $$ = ($1 < $3); }
	| expr GT expr			{ $$ = ($1 > $3); }
	| expr EQ expr			{ $$ = ($1 = $3); }
	| expr NEQ expr			{ $$ = ($1 != $3); }
	;

expr:
	  NUMBER				{ $$ = $1; }
	| IDENTIFIER			{ /*printf("Variable use: %s\n", $1);*/ $$ = 0; }
	| expr PLUS expr		{ $$ = $1 + $3; }
	| expr MINUS expr		{ $$ = $1 - $3; }
	| expr MUL expr			{ $$ = $1 * $3; }
	| expr DIV expr			{ $$ = $1 / $3; }
	| expr EQ expr			{ $$ = ($1 = $3); }
	| expr NEQ expr			{ $$ = ($1 != $3); }
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