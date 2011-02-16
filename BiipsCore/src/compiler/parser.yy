/* -*-C++-*- */
%{
#include <compiler/ParseTree.h>
#include "remap.h"
#include "parser_extra.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>
  
  void yyerror(const char *);
  int yylex();
  int yylex_destroy();
  extern int yylineno;
  
#define YYDEBUG 1
  
  static std::vector<ParseTree *> * _pvariables = 0;
  static ParseTree *_pdata = 0;
  static ParseTree *_prelations = 0;
  
  static void setName(ParseTree *p, std::string *name);
  static void setParameters(ParseTree *p, std::vector<ParseTree*> *parameters);
  static void setParameters(ParseTree *p, ParseTree *param1);
  static void setParameters(ParseTree *p, ParseTree *param1, 
			    ParseTree *param2);
  static void setParameters(ParseTree *p, ParseTree *param1, ParseTree *param2,
			    ParseTree *param3);
  static ParseTree *Truncated (ParseTree *left, ParseTree *right);
  %}

%defines
%union
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

%token VAR
%token DATA
%token MODEL
%token <stringptr> NAME
%token <stringptr> FUNC
%token <stringptr> SPECIAL
%token <stringptr> BADCHAR
%token IN
%token ARROW
%token FOR
%token GT GE LT LE EQ NE 
%token AND OR
%token LENGTH 
%token DIM

%token <val> DOUBLE
%token <val> INT

%left OR
%left AND
%left NOT
%nonassoc GT GE LT LE EQ NE
%left '+' '-'
%left '*' '/'
%left SPECIAL
%left NEG
%right '^'


%token ENDL

%type <ptree> node_dec
%type <ptree> expression var
%type <ptree> relation for_loop counter
%type <ptree> determ_relation stoch_relation  
%type <ptree> range_element
%type <ptree> distribution truncated relations
%type <pvec> dec_list relation_list expression_list dim_list 
%type <pvec> range_list 
%type <pvec> product sum

%expect 2

%%
input:   /* empty */
| model_stmt
| var_stmt model_stmt
| data_stmt model_stmt
| var_stmt data_stmt model_stmt
;

var_stmt: VAR dec_list { _pvariables = $2; }
| VAR dec_list ';' { _pvariables = $2; }
;

dec_list: node_dec { $$ = new std::vector<ParseTree*>(1, $1); }
| dec_list ',' node_dec { $$ = $1; $$->push_back($3); }
;

node_dec: NAME {
    $$ = new ParseTree(P_VAR, yylineno); setName($$, $1); 
}
| NAME '[' dim_list ']' {
    $$ = new ParseTree(P_VAR, yylineno); setName($$, $1);
    setParameters($$, $3);
}
;

dim_list: expression { $$ = new std::vector<ParseTree*>(1, $1); }
| dim_list ',' expression { $$=$1; $$->push_back($3); }
;

data_stmt: DATA '{' relation_list '}' {
    ParseTree *p = new ParseTree(P_RELATIONS, yylineno);
    setParameters(p, $3);
    _pdata = p;
}
;

model_stmt: MODEL '{' relation_list '}' { 
    ParseTree *p = new ParseTree(P_RELATIONS, yylineno);
    setParameters(p, $3);
    _prelations = p;
}
;
 
relations: '{' relation_list '}' {
    $$ = new ParseTree(P_RELATIONS, yylineno);
    setParameters($$, $2);
}
;

relation_list:	relation { $$ = new std::vector<ParseTree*>(1, $1); }
| relation_list relation { $$=$1; $$->push_back($2); }
;

relation: stoch_relation
| determ_relation 
| for_loop
| stoch_relation ';'
| determ_relation ';'
;

for_loop: counter relations {
    $$ = new ParseTree(P_FOR, yylineno);
    setParameters($$, $1, $2);
}
;

counter: FOR '(' NAME IN range_element ')' {
    $$ = new ParseTree(P_COUNTER, yylineno); setName($$, $3);
    setParameters($$, $5);
}
;

determ_relation: var ARROW expression {
    $$ = new ParseTree(P_DETRMREL, yylineno);
    setParameters($$, $1, $3);
} 
| FUNC '(' var ')' ARROW expression {

  /* 
     The link function is given using an S-style replacement function
     notation.  We need to turn this round so the inverse link
     function is applied to the RHS of the deterministic relation
  */
     
    ParseTree *p = new ParseTree(P_LINK, yylineno); setName(p, $1);
    setParameters(p, $6);
    
    $$ = new ParseTree(P_DETRMREL, yylineno);
    setParameters($$, $3, p);
}
;

stoch_relation:	var '~' distribution {
    $$ = new ParseTree(P_STOCHREL, yylineno); 
    setParameters($$, $1, $3);
 }
| var '~' distribution truncated {
    $$ = new ParseTree(P_STOCHREL, yylineno); 
    setParameters($$, $1, $3, $4);
  }
;

product: expression '*' expression {
  $$ = new std::vector<ParseTree*>;
  $$->push_back($1);
  $$->push_back($3);
}
| product '*' expression {
  //  This creates a shift-reduce conflict because in the expression
  //  A*B*C, (A*B) is a valid expression. By default, bison shifts,
  //  which is what we want. The warning is suppressed with the %expect
  //  declaration (See also sum: below).
  $$ = $1;
  $$->push_back($3);
}
;

sum: expression '+' expression {
  $$ = new std::vector<ParseTree*>;
  $$->push_back($1);
  $$->push_back($3);
}
| sum '+' expression {
  //  This creates a shift-reduce conflict. By default, bison shifts,
  //  which is what we want. The warning is suppressed with the %expect
  //  declaration (See also product: above).
  $$ = $1;
  $$->push_back($3);
}
;

expression: var 
| DOUBLE {$$ = new ParseTree(P_VALUE, yylineno); $$->setValue($1);}
| LENGTH '(' var ')' {
    $$ = new ParseTree(P_LENGTH, yylineno);
    setParameters($$,$3);
}
| DIM '(' var ')' {
    $$ = new ParseTree(P_DIM, yylineno);
    setParameters($$,$3);
}
| FUNC '(' expression_list ')' {
  $$ = new ParseTree(P_FUNCTION, yylineno); setName($$, $1);
  setParameters($$, $3);
}
| product {
    $$ = new ParseTree(P_FUNCTION, yylineno); $$->setName("*");
    setParameters($$, $1);
}
| expression '/' expression {
    $$ = new ParseTree(P_FUNCTION, yylineno); $$->setName("/");
    setParameters($$, $1, $3);
}
| sum {
    $$ = new ParseTree(P_FUNCTION, yylineno); $$->setName("+");
    setParameters($$, $1);
}
| expression '-' expression {
    $$ = new ParseTree(P_FUNCTION, yylineno); $$->setName("-");
    setParameters($$, $1, $3);
}
| '-' expression %prec NEG {
    $$ = new ParseTree(P_FUNCTION, yylineno); $$->setName("NEG");
    setParameters($$, $2);
}
| expression GT expression {
    $$ = new ParseTree(P_FUNCTION, yylineno); $$->setName(">");
    setParameters($$, $1, $3);
 }      
| expression GE expression {
    $$ = new ParseTree(P_FUNCTION, yylineno); $$->setName(">=");
    setParameters($$, $1, $3);
 }      
| expression LT expression {
    $$ = new ParseTree(P_FUNCTION, yylineno); $$->setName("<");
    setParameters($$, $1, $3);
 }      
| expression LE expression {
    $$ = new ParseTree(P_FUNCTION, yylineno); $$->setName("<=");
    setParameters($$, $1, $3);
 }      
| expression EQ expression {
    $$ = new ParseTree(P_FUNCTION, yylineno); $$->setName("==");
    setParameters($$, $1, $3);
 }      
| expression NE expression {
    $$ = new ParseTree(P_FUNCTION, yylineno); $$->setName("!=");
    setParameters($$, $1, $3);
}      
| expression AND expression {
    $$ = new ParseTree(P_FUNCTION, yylineno); $$->setName("&&");
    setParameters($$, $1, $3);
 }      
| expression OR expression {
    $$ = new ParseTree(P_FUNCTION, yylineno); $$->setName("||");
    setParameters($$, $1, $3);
}
| expression '^' expression {
    $$ = new ParseTree(P_FUNCTION, yylineno); $$->setName("^");
    setParameters($$, $1, $3);
  }
| expression SPECIAL expression {
    $$ = new ParseTree(P_FUNCTION, yylineno); setName($$, $2);
    setParameters($$, $1, $3);
 }
| '(' expression ')' { $$ = $2; }
;

expression_list: expression { $$ = new std::vector<ParseTree*>(1, $1); }
| expression_list ',' expression { $$=$1; $$->push_back($3); }
;

range_list: range_element { $$ = new std::vector<ParseTree*>(1, $1); }
| range_list ',' range_element { $$=$1; $$->push_back($3); }
;

range_element: {$$ = new ParseTree(P_RANGE, yylineno);}
| expression {$$ = new ParseTree(P_RANGE, yylineno); setParameters($$,$1);}
| expression ':' expression {
  $$ = new ParseTree(P_RANGE, yylineno); setParameters($$, $1, $3);
}
;

distribution: FUNC '(' expression_list ')'
{
  $$ = new ParseTree(P_DENSITY, yylineno); setName($$, $1);
  setParameters($$, $3);
}
;

truncated: 'T' '(' expression ','  expression ')' {$$ = Truncated($3,$5);}
| 'T' '(' ',' expression ')' {$$ = Truncated(0,$4);}
| 'T' '(' expression ',' ')' {$$ = Truncated($3,0);}
| 'T' '(' ',' ')' {$$ = Truncated(0,0);}
;

var: NAME {
  $$ = new ParseTree(P_VAR, yylineno); setName($$, $1);
}
| NAME '[' range_list ']' {
  $$ = new ParseTree(P_VAR, yylineno); setName($$, $1);
  setParameters($$, $3);
}
;

%%

static std::string error_buf;

void yyerror (const char *s)
{
    extern char * yytext;
    extern int yylineno;

    std::ostringstream msg;
    msg << std::string(s) << " on line " << yylineno << " near \"" << 
           std::string(yytext) << "\"";
    error_buf = msg.str();
}

static ParseTree *Truncated (ParseTree *left, ParseTree *right)
{
    ParseTree *p = new ParseTree(P_BOUNDS, yylineno);
    setParameters(p, left, right);
    return p;
}

void setName(ParseTree *p, std::string *name)
{
  /* 
     The scanner cannot return a string, because a string cannot be
     part of a union.  So instead the scanner returns a pointer to
     a newly allocated string.  This memory must be freed as soon
     as it is used.
  */
  p->setName(*name);
  delete name;
}
	

void setParameters(ParseTree *p, std::vector<ParseTree*> *parameters)
{
  /* 
     Same as setName (above).  The parser dynamically allocates
     vectors of (pointers to) parameters. These vectors must be
     deleted when we are done with them.
  */
  p->setParameters(*parameters);
  delete parameters; 
}

void setParameters(ParseTree *p, ParseTree *param1)
{
  /*
    Wrapper function that creates a vector containing param1
    to be passed to ParseTree::setParameters.
  */
  std::vector<ParseTree *> parameters(1, param1);
  p->setParameters(parameters);
}

void setParameters(ParseTree *p, ParseTree *param1, ParseTree *param2)
{
  /*
    Wrapper function that creates a vector containing param1
    and param2, to be passed to ParseTree::setParameters
  */
  std::vector<ParseTree *> parameters;
  parameters.push_back(param1);
  parameters.push_back(param2);
  p->setParameters(parameters);
}

void setParameters(ParseTree *p, ParseTree *param1, ParseTree *param2,
		   ParseTree *param3)
{
  /*
    Wrapper function that creates a vector containing param1, param2
    and param3, to be passed to ParseTree::setParameters
  */
  std::vector<ParseTree *> parameters;
  parameters.push_back(param1);
  parameters.push_back(param2);
  parameters.push_back(param3);
  p->setParameters(parameters);
}

int parse_bugs (std::FILE *file, std::vector<ParseTree*> * &dec_list, 
                ParseTree * &data, ParseTree * &relations,
		std::string &message)
{
    extern std::FILE *yyin;
    yyin = file;
    
    int val = 0;
    error_buf.clear();

    if (yyparse() == 0) {
	dec_list = _pvariables; 
	data = _pdata; 
	relations = _prelations;
    }
    else {
	message = error_buf;
	delete _pvariables; 
	delete _prelations; 
	delete _pdata;
	val = 1;
    }
    _pvariables = 0;
    _prelations = 0;
    _pdata = 0;

    yylex_destroy();

    return val;
}
