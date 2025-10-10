%{ // Пролог
#include <iostream>
#include <cstdlib>
#include <cstdio>

using namespace std;

extern int yylex(void);

void yyerror(char const* s) {
    cout << s << endl;
}

%}
// Секция объявлений

%union {
    int int_lit;
	int rune_lit;
    char *identifier;
	char *str_lit;
    float float_lit;
	bool bool_lit;
}

%token	PACKAGE
%token	CONST
%token 	FUNC
%token 	VAR

%token 	FMT

%token	INT
%token	FLOAT
%token	BOOL
%token 	STRING
%token 	RUNE

%token 	<int_lit>		INT_LIT
%token	<float_lit>		FLOAT_LIT
%token	<bool_lit>		BOOL_LIT
%token	<identifier>	ID
%token	<str_lit>		STRING_LIT
%token	<rune_lit>		RUNE_LIT

%right	'=' WALRUS
%left	OR
%left	AND
%left 	EQUAL NEQUAL '<' LESS_EQUAL '>' GREATER_EQUAL
%left	'+' '-'
%left	'*' '/'
%right	INC DEC '!' UMINUS


%start program

%%
// Секция правил грамматики

program			:	package_clause stmt_list
				;

package_clause	:	PACKAGE  ID
				;

stmt_list		:	stmt_list stmt
				|	stmt
				;

stmt			:	decl
				| 	simple_stmt
//				|	return_stmt
//				| 	break_stmt
//				| 	continue_stmt
//				| 	block
//				| 	if_stmt
//				| 	switch_stmt
//				| 	for_stmt
				;

simple_stmt		:	expr
				|	expr INC
				|	expr DEC
				|	expr_list '=' expr_list
				|	id_list WALRUS expr_list
				;

decl			:	const_decl
				:	var_decl
				;

const_decl		:	CONST const_spec
				|	'(' const_spec_list ')'
				;

const_spec_list	:	const_spec_list const_spec ';'
				|	const_spec ';'
				;

const_spec		:	id_list
				|	id_list '=' expr_list
				|	id_list type '=' expr_list
				;

id_list			:	id_list ',' ID
				|	ID
				;

type			:	type_name
				|	type_name type_args
				|	type_lit
				|	'(' type ')'
				;

type_name		:	INT
				|	FLOAT
				|	BOOL
				|	STRING
				|	RUNE
				|	qualified_ident
				;

qualified_ident	:	package_name '.' ID
				;

package_name	:	FMT
				;

type_args		:	'[' type_list ']'
				;

type_list		:	type_list ',' type
				|	type
				;

type_lit		:	array_type
				|	func_type
				|	slice_type
				;

array_type		:	'[' expr ']' type
				;

func_type		:	FUNC signature
				;

signature		:	params results
				|	params
				;

results			:	params
				|	type
				;

params			:	'(' param_list ')'
				;

param_list		:	param_list ',' param_decl
				|	param_decl
				;

param_decl		:	id_list type
				|	type
				;

slice_type		:	'[' ']' type
				;

var_decl		:	VAR var_spec
				|	VAR '(' var_spec_list ')'
				;

var_spec_list	:	var_spec_list ';' var_spec
				|	var_spec ';'
				;

var_spec		:	id_list type
				|	id_list type '=' expr_list
				|	id_list '=' expr_list
				;

expr_list		:	expr_list ',' expr
				|	expr
				;

expr			:	primary_expr
				|	expr '+' expr
				|	expr '-' expr
				|	expr '*' expr
				|	expr '/' expr
				|	expr EQUAL expr
				|	expr NEQUAL expr
				|	expr '<' expr
				|	expr '>' expr
				|	expr LESS_EQUAL expr
				|	expr GREATER_EQUAL expr
				|	expr AND expr
				|	expr OR expr
				|	'!' expr
				|	'-' expr	%prec UMINUS
				;

primary_expr	:	operand
				|	primary_expr '[' expr ']'
				|	primary_expr '[' ':' ']'
				|	primary_expr '[' expr ':' ']'
				|	primary_expr '[' ':' expr ']'
				|	primary_expr '[' expr ':' expr ']'
				|	primary_expr '[' ':' expr ':' expr ']'
				|	primary_expr '[' expr ':' expr ':' expr ']'
				;

operand			:	ID
				|	INT_LIT
				|	FLOAT_LIT
				|	RUNE_LIT
				|	STRING_LIT
				|	BOOL_LIT
				|	ID type_args
				|	qualified_ident
				|	qualified_ident type_args
				|	'(' expr ')'
				;


%%
// Секция пользовательского кода

