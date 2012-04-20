//                                               -*- lex -*-
/*
 * BiiPS software is a set of libraries for
 * Bayesian inference with interacting Particle Systems.
 * Copyright (C) Inria, 2012
 * Contributors: Adrien Todeschini, Francois Caron
 *
 * BiiPS is derived software based on:
 * JAGS, Copyright (C) Martyn Plummer, 2002-2010
 * SMCTC, Copyright (C) Adam M. Johansen, 2008-2009
 *
 * This file is part of BiiPS.
 *
 * BiiPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*! \file   scanner.ll
 * \brief
 *
 * \author  $LastChangedBy$
 * \date    $LastChangedDate$
 * \version $LastChangedRevision$
 * Id:      $Id$
 *
 * COPY: copied from JAGS
 */

/* -*-C++-*- */
%{
/* Note the use of trailing context in the rules for the functions,
   allowing function names to be used as variable names.  This occurs
   in the "otree" example where "mean" is used as a variable name.
*/

#include <compiler/ParseTree.h>
#include "remap.h"
#include "parser.h"

%}

EXPONENT	[eE][+-][0-9]+
BRACKET		[ \t]*\(

%x COMMENT

%option nounput
%option yylineno
%option nodefault

%%
"var"			return VAR;
"data"                  return DATA;
"model"			return MODEL;
"length"/{BRACKET}      return LENGTH;
"dim"/{BRACKET}         return DIM;
"for"/{BRACKET}        	return FOR;
"in"/{BRACKET}		return IN;
"in"                    return IN;
";"			return ';';
","			return ',';
":"			return ':';
"["			return '[';
"]"			return ']';
"("			return '(';
")"			return ')';
"{"			return '{';
"}"			return '}';
"<="                    return LE;
"<"                     return LT;
">="                    return GE;
">"                     return GT;
"&&"                    return AND;
"||"                    return OR;
"!="                    return NE;
"!"                     return NOT;
"=="                    return EQ;
"="			return '=';
"~"			return '~';
"<-"			return ARROW;
"+"			return '+';
"-"			return '-';
"^"                     return '^';
"**"                    return '^';
"*"			return '*';
"/"			return '/';

"T"/{BRACKET}           return 'T';

"%"+[^% \t\r\n]*"%" {
    /* Special operators, e.g. %*% for matrix multiplication */
    yylval.stringptr = new std::string(yytext);
    return SPECIAL;
}

"/*"           BEGIN(COMMENT);
<COMMENT>[^*]*          /* Eat up anything that's not a '*'   */
<COMMENT>"*"+[^*/n]*    /* Eat up '*'s not followed by a '/'  */
<COMMENT>"*"+"/"        BEGIN(INITIAL);

[ \t\r\n]+              /* Eat whitespace */
"#".*\n                 /* Eat comments */

([0-9]+){EXPONENT}? {
  yylval.val = atof(yytext); return DOUBLE;
}
([0-9]+"."[0-9]*){EXPONENT}? {
  yylval.val = atof(yytext); return DOUBLE;
}
("."[0-9]+){EXPONENT}? {
  yylval.val = atof(yytext); return DOUBLE;
}

([a-zA-Z]+)/{BRACKET} {
  yylval.stringptr = new std::string(yytext);
  return FUNC;
}

[a-zA-Z]+[a-zA-Z0-9\._]*	 {
  yylval.stringptr = new std::string(yytext);
  return NAME;
}

<<EOF>>	yyterminate();

. {
  /* Default rule for unmatched input.
     We return a BADCHAR which is not matched by any grammar rule,
     and so causes a syntax error.
  */
  return BADCHAR;
}

%%

int yywrap()
{
  return 1;
}
