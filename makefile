parser: 
	bison -d parser.y
	flex lexer.l
	gcc -o myparser parser.tab.c lex.yy.c -lfl
