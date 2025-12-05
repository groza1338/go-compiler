// Пролог
%code requires {
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "classes.h"

using namespace std;
}

%code provides {
    extern int yylex(void);
    void yyerror(const char* s);
}

// Секция объявлений

%{
    #include "golang_parser.hpp"
    ProgramNode* root = nullptr;
%}

%union {
    int int_lit;
	int rune_lit;
    string *identifier;
	string *str_lit;
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
    TypeNameNode *type_name_node;
    ValueNode *value_node;
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
%type   <type_name_node>            type_name
%type   <value_node>                literal_val

%right '=' WALRUS ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN
%left	OR
%left	AND
%left 	EQUAL NEQUAL '<' LESS_EQUAL '>' GREATER_EQUAL
%left	'+' '-'
%left	'*' '/'
%right	INC DEC '!' UMINUS '&'
%left   '.'
%nonassoc	'(' ')' '[' ']' '{' '}'


%start program

%%
// Секция правил грамматики

program			:	package_clause e_import_decl_list e_top_level_decl_list {root=ProgramNode::createNode($1, $2, $3);}
				;
				
e_import_decl_list
				:	import_decl_list {$$=$1;}
				|   %empty {$$=nullptr;}
				;
				
import_decl_list:	import_decl_list import_decl ';' {$$=ImportDeclListNode::addElemToList($1, $2);}
				|	import_decl ';' {$$=ImportDeclListNode::createList($1);}
				;
				
import_decl		:	IMPORT import_spec {$$=ImportDeclNode::createNode($2);}
				|	IMPORT '(' e_import_spec_list ')' {$$=ImportDeclNode::createNode($3);}
				;
				
e_import_spec_list
				:	import_spec_list {$$=$1;}
				|   %empty {$$=nullptr;}
				;
				
import_spec_list:	import_spec_list import_spec ';' {$$=ImportSpecListNode::addElemToList($1, $2);}
				|	import_spec ';' {$$=ImportSpecListNode::createList($1);}
				;
				
import_spec		:	STRING_LIT {$$=ImportSpecNode::createSimple(ValueNode::createString($1));}
				|	'.' STRING_LIT {$$=ImportSpecNode::createPoint(ValueNode::createString($2));}
				|	ID STRING_LIT {$$=ImportSpecNode::createNamed(ValueNode::createString($1), ValueNode::createString($2));}
				;
				
e_top_level_decl_list
				:	top_level_decl_list {$$=$1;}
				|   %empty {$$=nullptr;}
				;
				
top_level_decl_list
				:	top_level_decl_list top_level_decl ';' {$$=TopLevelDeclListNode::addElemToList($1, $2);}
				|	top_level_decl ';' {$$=TopLevelDeclListNode::createList($1);}
				;
				
top_level_decl	:	decl {$$=TopLevelDeclNode::createTopLevelDecl($1);}
				|	func_decl {$$=TopLevelDeclNode::createTopLevelDecl($1);}
				;
				
func_decl		:	FUNC ID signature {$$=FuncDeclNode::createFuncDecl(ValueNode::createString($2), $3, nullptr);}
				|	FUNC ID signature block {$$=FuncDeclNode::createFuncDecl(ValueNode::createString($2), $3, $4);}
				;

package_clause	:	PACKAGE ID ';' {$$=PackageClauseNode::createNode(ValueNode::createString($2));}
				;

e_stmt_list     :   stmt_list {$$=$1;}
                |   %empty {$$=nullptr;}
                ;

stmt_list		:	stmt_list stmt {$$=StmtListNode::addStmtToList($1, $2);}
				|	stmt {$$=StmtListNode::createStmtList($1);}
				;
				
				
stmt			:	decl ';' {$$=StmtNode::createDecl($1);}
				| 	simple_stmt ';' {$$=StmtNode::createSimple($1);}
				|	return_stmt ';' {$$=$1;}
				| 	BREAK ';' {$$=StmtNode::createBreak();}
				| 	CONTINUE ';' {$$=StmtNode::createContinue();}
				| 	block ';' {$$=$1;}
				| 	if_stmt ';' {$$=$1;}
				| 	switch_stmt ';' {$$=$1;}
				| 	for_stmt ';' {$$=$1;}
				|   ';' {$$=nullptr;}
				;

e_simple_stmt   :   simple_stmt {$$=$1;}
                |   %empty {$$=nullptr;}
                ;
				
simple_stmt		:	expr {$$=SimpleStmtNode::createExpr($1);}
				|	expr INC {$$=SimpleStmtNode::createInc($1);}
				|	expr DEC {$$=SimpleStmtNode::createDec($1);}
				|	expr_list '=' expr_list {$$=SimpleStmtNode::createAssign($1, $3);}
				|	expr_list ADD_ASSIGN expr_list {$$=SimpleStmtNode::createAddAssign($1, $3);}
				|	expr_list SUB_ASSIGN expr_list {$$=SimpleStmtNode::createSubAssign($1, $3);}
				|	expr_list MUL_ASSIGN expr_list {$$=SimpleStmtNode::createMulAssign($1, $3);}
				|	expr_list DIV_ASSIGN expr_list {$$=SimpleStmtNode::createDivAssign($1, $3);}
				|	expr_list WALRUS expr_list {$$=SimpleStmtNode::createShortVarDecl($1, $3);}
				;
				
return_stmt		:	RETURN {$$=StmtNode::createReturn(nullptr);}
				|	RETURN expr_list {$$=StmtNode::createReturn($2);}
				;
				
block			:	'{' e_stmt_list '}' {$$=StmtNode::createBlock($2);}
				;

decl			:	CONST const_spec {$$=DeclNode::createDecl($2);}
                |   CONST '(' const_spec_list ')' {$$=DeclNode::createDecl($3);}
				|	VAR var_spec {$$=DeclNode::createDecl($2);}
                |	VAR '(' var_spec_list ')' {$$=DeclNode::createDecl($3);}
				;
				
if_stmt			:	IF expr block {$$=StmtNode::createIf(nullptr, $2, $3, nullptr);}
				|	IF simple_stmt ';' expr block {$$=StmtNode::createIf($2, $4, $5, nullptr);}
				|	IF expr block ELSE if_stmt {$$=StmtNode::createIf(nullptr, $2, $3, $5);}
				|	IF expr block ELSE block {$$=StmtNode::createIf(nullptr, $2, $3, $5);}
				|	IF simple_stmt ';' expr block ELSE if_stmt {$$=StmtNode::createIf($2, $4, $5, $7);}
				|	IF simple_stmt ';' expr block ELSE block {$$=StmtNode::createIf($2, $4, $5, $7);}
				;
				
switch_stmt		:	SWITCH '{' e_expr_case_clause_list '}' {$$=StmtNode::createSwitch(nullptr, nullptr, $3);}
				|	SWITCH simple_stmt ';' '{' e_expr_case_clause_list '}' {$$=StmtNode::createSwitch($2, nullptr, $5);}
				|	SWITCH expr '{' e_expr_case_clause_list '}' {$$=StmtNode::createSwitch(nullptr, $2, $4);}
				|	SWITCH simple_stmt ';' expr '{' e_expr_case_clause_list '}' {$$=StmtNode::createSwitch($2, $4, $6);}
				;
				
for_stmt		:	FOR block {$$=StmtNode::createFor(nullptr, $2);}
				|	FOR expr block {$$=StmtNode::createFor($2, $3);}
				|	FOR e_simple_stmt ';' e_expr ';' e_simple_stmt block {$$=StmtNode::createFor($2, $4, $6, $7);}
				|	FOR RANGE expr block {$$=StmtNode::createFor(nullptr, $3, $4);}
				|	FOR expr_list '=' RANGE expr block {$$=StmtNode::createFor($2, $5, $6);}
				|	FOR expr_list WALRUS RANGE expr block {$$=StmtNode::createFor($2, $5, $6);}
				;

e_expr_case_clause_list
				:	expr_case_clause_list {$$=$1;}
				|   %empty {$$=nullptr;}
				;

expr_case_clause_list
				:	expr_case_clause_list expr_case_clause {$$=CaseListNode::addCaseToList($1, $2);}
				|	expr_case_clause {$$=CaseListNode::createCaseList($1);}
				;
				
expr_case_clause:	CASE expr_list ':' stmt_list {$$=CaseNode::createCase($2, $4);}
                |	DEFAULT ':' stmt_list {$$=CaseNode::createCase(nullptr, $3);}
				;
				
const_spec_list	:	const_spec_list const_spec ';' {$$=ConstSpecListNode::addConstSpecToList($1, $2);}
				|	const_spec ';' {$$=ConstSpecListNode::createConstSpecList($1);}
				;

const_spec		:	id_list {$$=ConstSpecNode::createConstSpec($1, nullptr, nullptr);}
				|	id_list '=' expr_list {$$=ConstSpecNode::createConstSpec($1, nullptr, $3);}
				|	id_list type '=' expr_list {$$=ConstSpecNode::createConstSpec($1, $2, $4);}
				;
				
id_list			:	id_list ',' ID {$$=IdListNode::addIdToList($1, ValueNode::createString($3));}
				|	ID {$$=IdListNode::createIdList(ValueNode::createString($1));}
				;
				
type			:	type_name {$$=TypeNode::createNamedType($1);}
				|	'[' expr ']' type {$$=TypeNode::createArrayType($2, $4);}
                |	FUNC signature {$$=TypeNode::createFuncType($2);}
                |	'[' ']' type {$$=TypeNode::createSliceType($3);}
				;
				
type_name		:	INT {$$=TypeNameNode::createTypeInt();}
				|	FLOAT {$$=TypeNameNode::createTypeFloat();}
				|	BOOL {$$=TypeNameNode::createTypeBool();}
				|	STRING {$$=TypeNameNode::createTypeString();}
				|	RUNE {$$=TypeNameNode::createTypeRune();}
				;
				
signature		:	'(' e_param_list ')' results {$$=SignatureNode::createSignature($2, $4);}
				|	'(' e_param_list ')' {$$=SignatureNode::createSignature($2, nullptr);}
				;
				
results			:	'(' param_list ')' {$$=ResultNode::createResult($2);}
				|	type {$$=ResultNode::createResult($1);}
				;

e_param_list    :   param_list {$$=$1;}
                |   %empty {$$=nullptr;}
                ;
				
param_list		:	param_list ',' param_decl {$$=ParamDeclListNode::addParamDeclToList($1, $3);}
				|	param_decl {$$=ParamDeclListNode::createParamDeclList($1);}
				;
				
param_decl		:	id_list type {$$=ParamDeclNode::createParamDecl($1, $2);}
				|	type {$$=ParamDeclNode::createParamDecl(nullptr, $1);}
				;

var_spec_list	:	var_spec_list ';' var_spec {$$=VarSpecListNode::addVarSpecToList($1, $3);}
				|	var_spec ';' {$$=VarSpecListNode::createVarSpecList($1);}
				;
				
var_spec		:	id_list type {$$=VarSpecNode::createVarSpec($1, $2, nullptr);}
				|	id_list type '=' expr_list {$$=VarSpecNode::createVarSpec($1, $2, $4);}
				|	id_list '=' expr_list {$$=VarSpecNode::createVarSpec($1, nullptr, $3);}
				;
				
expr_list		:	expr_list ',' expr {$$=ExprListNode::addExprToList($1, $3);}
				|	expr {$$=ExprListNode::createExprList($1);}
				;

e_expr          :   expr {$$=$1;}
                |   %empty {$$=nullptr;}
                ;

expr			:	ID {$$=ExprNode::createIdentifier(ValueNode::createString($1));}
                |	IOTA {$$=ExprNode::createIota();}
                |	'(' expr ')' {$$=$2;}
                |	literal_val {$$=ExprNode::createLiteralVal($1);}
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
				|	'&' expr {$$=ExprNode::createAddressOf($2);}
				|	expr '.' ID {$$=ExprNode::createSelector($1, ValueNode::createString($3));}
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

literal_val     :   INT_LIT {$$=ValueNode::createInt($1);}
                |   FLOAT_LIT {$$=ValueNode::createFloat($1);}
                |	RUNE_LIT {$$=ValueNode::createRune($1);}
                |	STRING_LIT {$$=ValueNode::createString($1);}
                |	BOOL_LIT {$$=ValueNode::createBool($1);}
                ;

%%

void yyerror(const char* s) {
    cout << s << endl;
}
// Секция пользовательского кода
