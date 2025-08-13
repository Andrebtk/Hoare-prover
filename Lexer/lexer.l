%{ 
	#include <stdio.h>
	#include "../Ast/ast.h"
	#include "../Parser/parser.tab.h"  /* not parser.y */

%}


%%

"//"[^\r\n]*   { /* skip line comment */ }


[0-9]+ { 
	yylval.num = atoi(yytext);
	return NUMBER;
}

"/" {
	return DIV;
}

"min" {
	return MIN;
}

"max" {
	return MAX;
}


"fact" {
	return FACT;
}

"->" {
	return IMPLY;
}

"==" { 
	return EQ;
}

"invariant" {
	return INVARIANT;
}

"variant" {
	return VARIANT;
}


"=" { 
	return ASSIGN;
}

"," {
	return COMMA;
}

"!=" { 
	return NEQ;
}

"+" { 
	return PLUS;
}

"-" { 
	return MINUS;
}

"*"						{ 
	return MUL;
}

[ \t]+					;

\n 						;


"true"    return TRUE;
"false"   return FALSE;

"TRUE"    return TRUE;
"FALSE"   return FALSE;

";" { 
	return SEMICOLON;
}

"if" {
	return IF;
}

"else" { 
	return ELSE;
}

"while" {
	return WHILE;
}

"(" {
	return LPAREN;
}

")" {
	return RPAREN;
}

"{" {
	return LBRACE;
}

"}" {
	return RBRACE;
}

"<=" {
	return LE;
}

">=" {
	return GE;
}

"<" {
	return LT;
}

">" {
	return GT;
}



"PRECONDITION:" {
	return PRECOND;
}

"POSTCONDITION:" {
	return POSTCOND;
}


"AND" {
	return AND;
}

"and" {
	return AND;
}

"OR" {
	return OR;
}

"or" {
	return OR;
}

"NOT" {
	return NOT;
}

"not" {
	return NOT;
}




[a-zA-Z][a-zA-Z0-9]* { 
	yylval.id = strdup(yytext);
	return IDENTIFIER;
}



%%


int yywrap() {
	return 1;
}

