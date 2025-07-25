main: parser/parser.tab.c parser/parser.tab.h lexer/lex.yy.c
	gcc -I. -o myparser parser/parser.tab.c lexer/lex.yy.c -lfl

parser/parser.tab.c parser/parser.tab.h: parser/parser.y 
	bison -d -o parser/parser.tab.c parser/parser.y


lexer/lex.yy.c: lexer/lexer.l parser/parser.tab.h
	flex -o lexer/lex.yy.c lexer/lexer.l

clean:
	rm myparser parser/parser.tab.c parser/parser.tab.h lexer/lex.yy.c