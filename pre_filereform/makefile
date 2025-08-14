# Nom de l'exécutable
TARGET = myparser

# Sources
SOURCES = Parser/parser.tab.c \
          Lexer/lex.yy.c \
          Ast/ast.c \
          Hashmap/hashmap.c \
          Hoare/hoare.c \
          Z3/z3_helpers.c

# Règle par défaut
all: $(TARGET)

# Compilation finale
$(TARGET): $(SOURCES)
	gcc \
	    -I. -IAst -IHashmap -IHoare -IZ3 -IParser -ILexer \
	    -o $(TARGET) $(SOURCES) -lz3 -lfl

# Génération du parser
Parser/parser.tab.c Parser/parser.tab.h: Parser/parser.y
	bison -t -d -o Parser/parser.tab.c Parser/parser.y

# Génération du lexer
Lexer/lex.yy.c: Lexer/lexer.l Parser/parser.tab.h
	flex -o Lexer/lex.yy.c Lexer/lexer.l

# Nettoyage
clean:
	rm -f $(TARGET) Parser/parser.tab.c Parser/parser.tab.h Lexer/lex.yy.c

.PHONY: all clean
