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
	ExprNode *expr_node;
	ExprListNode *expr_list_node;
	StmtListNode *stmt_list_node;
	StmtNode *stmt_node;
	CaseNode *case_node;
	CaseListNode *case_list_node;
	SimpleStmtNode *simple_stmt_node;
	IdListNode *id_list_node;
	TypeNode *type_node;
	ParamDeclNode *param_decl_node;
	ParamDeclListNode *param_decl_list_node;
	SignatureNode *signature_node;
	ResultNode *result_node;
	VarSpecNode *var_spec_node;
	VarSpecListNode *var_spec_list_node;
	ConstSpecNode *const_spec_node;
	ConstSpecListNode *const_spec_list_node;
	DeclNode *decl_node;
	FuncDeclNode *func_decl_node;
    TopLevelDeclNode *top_level_decl_node;
    TopLevelDeclListNode *top_level_decl_list_node;
    PackageClauseNode *package_clause_node;
    ImportSpecNode *import_spec_node;
    ImportSpecListNode *import_spec_list_node;
    ImportDeclNode *import_decl_node;
    ImportDeclListNode *import_decl_list_node;
    ProgramNode *program_node;
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

%token  ADD_ASSIGN
%token  SUB_ASSIGN
%token  MUL_ASSIGN
%token  DIV_ASSIGN

%token  IOTA

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

%type   <expr_node>                 e_expr expr
%type   <expr_list_node>            expr_list
%type   <stmt_list_node>            e_stmt_list stmt_list
%type   <stmt_node>                 stmt return_stmt block if_stmt switch_stmt for_stmt
%type   <case_node>                 expr_case_clause
%type   <case_list_node>            e_expr_case_clause_list expr_case_clause_list
%type   <simple_stmt_node>          e_simple_stmt simple_stmt
%type   <id_list_node>              id_list
%type   <type_node>                 type
%type   <param_decl_node>           param_decl
%type   <param_decl_list_node>      e_param_list param_list
%type   <signature_node>            signature
%type   <result_node>               results
%type   <var_spec_node>             var_spec
%type   <var_spec_list_node>        var_spec_list
%type   <const_spec_node>           const_spec
%type   <const_spec_list_node>      const_spec_list
%type   <decl_node>                 decl
%type   <func_decl_node>            func_decl
%type   <top_level_decl_node>       top_level_decl
%type   <top_level_decl_list_node>  e_top_level_decl_list top_level_decl_list
%type   <package_clause_node>       package_clause
%type   <import_spec_node>          import_spec
%type   <import_spec_list_node>     e_import_spec_list import_spec_list
%type   <import_decl_node>          import_decl
%type   <import_decl_list_node>     e_import_decl_list import_decl_list
%type   <program_node>              program

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

program			:	package_clause e_import_decl_list e_top_level_decl_list
				;
				
e_import_decl_list
				:	import_decl_list ';'
				|   %empty
				;
				
import_decl_list:	import_decl_list import_decl ';'
				|	import_decl ';'
				;
				
import_decl		:	IMPORT import_spec 
				|	IMPORT '(' e_import_spec_list ')'
				;
				
e_import_spec_list
				:	import_spec_list
				|   %empty
				;
				
import_spec_list:	import_spec_list import_spec ';'
				|	import_spec ';'
				;
				
import_spec		:	STRING_LIT
				|	'.' STRING_LIT
				|	ID STRING_LIT
				;
				
e_top_level_decl_list
				:	top_level_decl_list
				|   %empty
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

package_clause	:	PACKAGE ID ';'
				;

e_stmt_list     :   stmt_list 
                |   %empty
                ;

stmt_list		:	stmt_list stmt 
				|	stmt 
				;
				
				
stmt			:	decl ';'
				| 	simple_stmt ';' 
				|	return_stmt ';' 
				| 	BREAK ';' 
				| 	CONTINUE ';' 
				| 	block ';' 
				| 	if_stmt ';' 
				| 	switch_stmt ';'
				| 	for_stmt ';'
				|   ';'
				;

e_simple_stmt   :   simple_stmt
                |   %empty
                ;
				
simple_stmt		:	expr 
				|	expr INC 
				|	expr DEC 
				|	expr_list '=' expr_list 
				|	expr_list ADD_ASSIGN expr_list 
				|	expr_list SUB_ASSIGN expr_list 
				|	expr_list MUL_ASSIGN expr_list 
				|	expr_list DIV_ASSIGN expr_list 
				|	expr_list WALRUS expr_list
				;
				
return_stmt		:	RETURN 
				|	RETURN expr_list 
				;
				
block			:	'{' e_stmt_list '}' 
				;

decl			:	CONST const_spec
                |   CONST '(' const_spec_list ')'
				|	VAR var_spec
                |	VAR '(' var_spec_list ')'
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
				|	FOR e_simple_stmt ';' e_expr ';' e_simple_stmt block
				|	FOR RANGE expr block
				|	FOR expr_list '=' RANGE expr block
				|	FOR expr_list WALRUS RANGE expr block
				;

e_expr_case_clause_list
				:	expr_case_clause_list 
				|   %empty
				;

expr_case_clause_list
				:	expr_case_clause_list expr_case_clause 
				|	expr_case_clause 
				;
				
expr_case_clause:	CASE expr_list ':' stmt_list
                |	DEFAULT ':' stmt_list
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
				|	'[' expr ']' type
                |	FUNC signature
                |	'[' ']' type
				;
				
type_name		:	INT
				|	FLOAT
				|	BOOL
				|	STRING
				|	RUNE
				;
				
signature		:	'(' e_param_list ')' results
				|	'(' e_param_list ')'
				;
				
results			:	'(' param_list ')'
				|	type
				;

e_param_list    :   param_list
                |   %empty
                ;
				
param_list		:	param_list ',' param_decl
				|	param_decl
				;
				
param_decl		:	id_list type
				|	type
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

e_expr          :   expr
                |   %empty
                ;

expr			:	ID 
					|	IOTA 
					|	'(' expr ')' 
					|	INT_LIT 
				|	FLOAT_LIT 
				|	RUNE_LIT 
				|	STRING_LIT 
				|	BOOL_LIT 
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
				|	expr '[' expr ']' 
				|	expr '[' ':' ']' 
				|	expr '[' expr ':' ']' 
				|	expr '[' ':' expr ']' 
				|	expr '[' expr ':' expr ']' 
				|	expr '[' ':' expr ':' expr ']' 
				|	expr '[' expr ':' expr ':' expr ']' 
				|	expr '(' ')' 
				|	expr '(' expr_list ')' 
				;

%%
// Секция пользовательского кода
