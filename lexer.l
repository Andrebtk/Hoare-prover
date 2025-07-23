%{ 
	#include <stdio.h>
%}


%%
[a-zA-Z][a-zA-Z0-9]*	{ printf("ID: %s \n", yytext); }
[0-9]+					{ printf("NUMBER %s\n", yytext); }
"="						{ printf("ASSIGN %s\n", yytext); }
"!="					{ printf("NOT EQUAL %s\n", yytext); }
"+"						{ printf("PLUS %s\n", yytext); }
"-"						{ printf("MINUS %s\n", yytext); }
"*"						{ printf("MULT %s\n", yytext); }
[ \t]+					;
\n						{ printf("NEW LINE\n\n"); }
";"						{ printf("SEMICOLON: ;\n"); }

%%


int yywrap() {
	return 1;
}
