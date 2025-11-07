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
%token 	IMPORT
%token	CONST
%token 	FUNC
%token 	VAR
%token 	RETURN
%token	BREAK
%token 	CONTINUE
%token 	IF
%token 	ELSE
%token 	SWITCH
%token	CASE
%token	DEFAULT
%token	FOR
%token 	RANGE

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
%nonassoc	'(' ')' '[' ']' '{' '}'


%start program

%%
// Секция правил грамматики

program			:	package_clause ';' e_import_decl_list ';' e_top_level_decl_list
				;
				
e_import_decl_list
				:	import_decl_list
				|
				;
				
import_decl_list:	import_decl_list import_decl ';'
				|	import_decl ';'
				;
				
import_decl		:	IMPORT import_spec 
				|	IMPORT '(' e_import_spec_list ')'
				;
				
e_import_spec_list
				:	import_spec_list
				|
				;
				
import_spec_list:	import_spec_list import_spec ';'
				|	import_spec ';'
				;
				
import_spec		:	STRING_LIT
				|	'.' STRING_LIT
				|	STRING_LIT STRING_LIT
				;
				
e_top_level_decl_list
				:	top_level_decl_list
				|
				;
				
top_level_decl_list
				:	top_level_decl_list top_level_decl ';'
				|	top_level_decl ';'
				;
				
top_level_decl	:	decl
				|	func_decl
				;
				
func_decl		:	FUNC ID signature
				|	FUNC ID signature block
				;

package_clause	:	PACKAGE ID
				;

e_stmt_list     :   stmt_list {$$=$1;}
                |   {$$=nullptr;}
                ;

stmt_list		:	stmt_list stmt {$$=StmtListNode::addStmtToList($1, $2);}
				|	stmt {$$=StmtListNode::createStmtList($1);}
				;
				
				
stmt			:	decl ';'
				| 	simple_stmt ';'
				|	return_stmt ';' {$$=$1;}
				| 	BREAK ';' {$$=StmtNode::createBreak();}
				| 	CONTINUE ';' {$$=StmtNode::createContinue();}
				| 	block ';' {$$=$1;}
				| 	if_stmt ';'
				| 	switch_stmt ';'
				| 	for_stmt ';'
				|   ';'
				;
				
simple_stmt		:	expr
				|	expr INC
				|	expr DEC
				|	expr_list '=' expr_list
				|	expr_list WALRUS expr_list
				;
				
return_stmt		:	RETURN {$$=StmtNode::createReturn(nullptr);}
				|	RETURN expr_list {$$=StmtNode::createReturn($2);}
				;
				
block			:	'{' e_stmt_list '}' {$$=$2;}
				;

decl			:	const_decl
				|	var_decl
				;
				
if_stmt			:	IF expr block 
				|	IF simple_stmt ';' expr block
				|	IF expr block ELSE if_stmt
				|	IF expr block ELSE block
				|	IF simple_stmt ';' expr block ELSE if_stmt
				|	IF simple_stmt ';' expr block ELSE block
				;
				
switch_stmt		:	SWITCH '{' e_expr_case_clause_list '}'
				|	SWITCH simple_stmt ';' '{' e_expr_case_clause_list '}'
				|	SWITCH expr '{' e_expr_case_clause_list '}'
				|	SWITCH simple_stmt ';' expr '{' e_expr_case_clause_list '}'
				;
				
for_stmt		:	FOR block
				|	FOR expr block
				|	FOR for_clause block
				|	FOR range_clause block
				;
				
for_clause		:	';' ';'
				|	simple_stmt ';' ';'
				|	simple_stmt ';' expr ';'
				|	simple_stmt ';' ';' simple_stmt
				|	simple_stmt ';' expr ';' simple_stmt
				|	';' expr ';'
				|	';' expr ';' simple_stmt
				|	';' ';' simple_stmt
				;
				
range_clause	:	RANGE expr
				|	expr_list '=' RANGE expr
				|	expr_list WALRUS RANGE expr
				;
				
e_expr_case_clause_list
				:	expr_case_clause_list
				|
				;

expr_case_clause_list
				:	expr_case_clause_list expr_case_clause
				|	expr_case_clause
				;
				
expr_case_clause:	expr_switch_case ':' stmt_list
				;
					
expr_switch_case:	CASE expr_list
				|	DEFAULT
				;

const_decl		:	CONST const_spec
				|	CONST '(' const_spec_list ')'
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
				|	type_name '[' type_list ']'
				|	type_lit
				;
				
type_name		:	INT
				|	FLOAT
				|	BOOL
				|	STRING
				|	RUNE
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
				
expr_list		:	expr_list ',' expr {$$=ExprListNode::addExprToList($1, $3);}
				|	expr {$$=ExprListNode::createExprList($1);}
				;
				
expr			:	ID {$$=ExprNode::createIdentifier($1);}
				|	'(' expr ')' {$$=$1;}
				|	INT_LIT {$$=ExprNode::createIntLiteral($1);}
				|	FLOAT_LIT {$$=ExprNode::createFloatLiteral($1);}
				|	RUNE_LIT {$$=ExprNode::createRuneLiteral($1);}
				|	STRING_LIT {$$=ExprNode::createStringLiteral($1);}
				|	BOOL_LIT {$$=ExprNode::createBoolLiteral($1);}
				|	expr '+' expr {$$=ExprNode::createSummary($1, $3);}
				|	expr '-' expr {$$=ExprNode::createSubtraction($1, $3);}
				|	expr '*' expr {$$=ExprNode::createMultiplication($1, $3);}
				|	expr '/' expr {$$=ExprNode::createDivision($1, $3);}
				|	expr EQUAL expr {$$=ExprNode::createEqual($1, $3);}
				|	expr NEQUAL expr {$$=ExprNode::createNotEqual($1, $3);}
				|	expr '<' expr {$$=ExprNode::createLess($1, $3);}
				|	expr '>' expr {$$=ExprNode::createGreater($1, $3);}
				|	expr LESS_EQUAL expr {$$=ExprNode::createLessOrEqual($1, $3);}
				|	expr GREATER_EQUAL expr {$$=ExprNode::createGreaterOrEqual($1, $3);}
				|	expr AND expr {$$=ExprNode::createAnd($1, $3);}
				|	expr OR expr {$$=ExprNode::createOr($1, $3);}
				|	'!' expr {$$=ExprNode::createNot($2);}
				|	'-' expr	%prec UMINUS {$$=ExprNode::createUnaryMinus($2);}
				|	expr '[' expr ']' {$$=ExprNode::createElementAccess($1, $3);}
				|	expr '[' ':' ']' {$$=ExprNode::createSlice($1, nullptr, nullptr, nullptr);}
				|	expr '[' expr ':' ']' {$$=ExprNode::createSlice($1, $3, nullptr, nullptr);}
				|	expr '[' ':' expr ']' {$$=ExprNode::createSlice($1, nullptr, $4, nullptr);}
				|	expr '[' expr ':' expr ']' {$$=ExprNode::createSlice($1, $3, $5, nullptr);}
				|	expr '[' ':' expr ':' expr ']' {$$=ExprNode::createSlice($1, nullptr, $4, $6);}
				|	expr '[' expr ':' expr ':' expr ']' {$$=ExprNode::createSlice($1, $3, $5, $7);}
				|	expr '(' ')' {$$=ExprNode::createFunctionCall($1, nullptr);}
				|	expr '(' expr_list ')' {$$=ExprNode::createFunctionCall($1, $3);}
				;

%%
// Секция пользовательского кода

