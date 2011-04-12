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
     VAR = 258,
     DATA = 259,
     MODEL = 260,
     NAME = 261,
     FUNC = 262,
     SPECIAL = 263,
     BADCHAR = 264,
     IN = 265,
     ARROW = 266,
     FOR = 267,
     GT = 268,
     GE = 269,
     LT = 270,
     LE = 271,
     EQ = 272,
     NE = 273,
     AND = 274,
     OR = 275,
     LENGTH = 276,
     DIM = 277,
     DOUBLE = 278,
     INT = 279,
     NOT = 280,
     NEG = 281,
     ENDL = 282
   };
#endif
/* Tokens.  */
#define VAR 258
#define DATA 259
#define MODEL 260
#define NAME 261
#define FUNC 262
#define SPECIAL 263
#define BADCHAR 264
#define IN 265
#define ARROW 266
#define FOR 267
#define GT 268
#define GE 269
#define LT 270
#define LE 271
#define EQ 272
#define NE 273
#define AND 274
#define OR 275
#define LENGTH 276
#define DIM 277
#define DOUBLE 278
#define INT 279
#define NOT 280
#define NEG 281
#define ENDL 282




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 37 "parser.yy"
{
  /*
    We can't put any C++ class that requires a constructor in
    union. In particular, we can't have strings.  To get round this
    limitation, we put a pointer to a string in the union, and then
    get the lexical analyzer to return a pointer to a dynamically
    allocated string.  The parser frees the memory when it is done.
    This is tedious, but the alternative is to copy character
    arrays using, e.g. strncpy instead.
  */
  double val;
  std::string *stringptr;
  ParseTree *ptree;
  std::vector<ParseTree*> *pvec;
}
/* Line 1489 of yacc.c.  */
#line 119 "../../../../JAGS/src/lib/compiler/parser.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

