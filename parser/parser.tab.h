/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IF = 258,
     WHILE = 259,
     SEMICOLON = 260,
     LPAREN = 261,
     RPAREN = 262,
     LBRACE = 263,
     RBRACE = 264,
     PLUS = 265,
     MINUS = 266,
     MUL = 267,
     DIV = 268,
     LT = 269,
     GT = 270,
     IDENTIFIER = 271,
     NUMBER = 272,
     ASSIGN = 273,
     EQ = 274,
     NEQ = 275
   };
#endif
/* Tokens.  */
#define IF 258
#define WHILE 259
#define SEMICOLON 260
#define LPAREN 261
#define RPAREN 262
#define LBRACE 263
#define RBRACE 264
#define PLUS 265
#define MINUS 266
#define MUL 267
#define DIV 268
#define LT 269
#define GT 270
#define IDENTIFIER 271
#define NUMBER 272
#define ASSIGN 273
#define EQ 274
#define NEQ 275




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 15 "parser/parser.y"
{
	int num;
	char *id;
	ASTNode* node;
	DLL* dll;
}
/* Line 1529 of yacc.c.  */
#line 96 "parser/parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

