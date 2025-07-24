%{ 
	#include <stdio.h>
	#include "parser.tab.h"
%}


%%


[0-9]+ { 
	yylval.num = atoi(yytext);
	return NUMBER;

}

"==" { 
	return EQ;
}

"=" { 
	return ASSIGN;
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


";" { 
	return SEMICOLON;
}

"if" {
	return IF;
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

"<" {
	return LT;
}

">" {
	return GT;
}

[a-zA-Z][a-zA-Z0-9]* { 
	yylval.id = strdup(yytext);
	return IDENTIFIER;
}

%%


int yywrap() {
	return 1;
}
