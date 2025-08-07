main: parser/parser.tab.c parser/parser.tab.h lexer/lex.yy.c data-struct.c
	gcc -I. -o myparser parser/parser.tab.c lexer/lex.yy.c data-struct.c

parser/parser.tab.c parser/parser.tab.h: parser/parser.y 
	bison -t -d -o parser/parser.tab.c parser/parser.y


lexer/lex.yy.c: lexer/lexer.l parser/parser.tab.h
	flex -o lexer/lex.yy.c lexer/lexer.l

data-struct: 
	gcc -o data-struct data-struct.c 


clean:
	rm myparser parser/parser.tab.c parser/parser.tab.h lexer/lex.yy.c