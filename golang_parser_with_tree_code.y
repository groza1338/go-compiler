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

program			:	package_clause ';' e_import_decl_list ';' e_top_level_decl_list {$$=ProgramNode::createNode($1, $3, $5);}
				;
				
e_import_decl_list
				:	import_decl_list {$$=$1;}
				|   {$$=nullptr;}
				;
				
import_decl_list:	import_decl_list import_decl ';' {$$=ImportDeclListNode::addElemToList($1, $2);}
				|	import_decl ';' {$$=ImportDeclListNode::createList($1);}
				;
				
import_decl		:	IMPORT import_spec {$$=ImportDeclNode::createNode($2);}
				|	IMPORT '(' e_import_spec_list ')' {$$=ImportDeclNode::createNode($3);}
				;
				
e_import_spec_list
				:	import_spec_list {$$=$1;}
				|   {$$=nullptr;}
				;
				
import_spec_list:	import_spec_list import_spec ';' {$$=ImportSpecListNode::addElemToList($1, $2);}
				|	import_spec ';' {$$=ImportSpecListNode::createList($1);}
				;
				
import_spec		:	STRING_LIT {$$=ImportSpecNode::createSimple($1);}
				|	'.' STRING_LIT {$$=ImportSpecNode::createPoint($2);}
				|	ID STRING_LIT {$$=ImportSpecNode::createNamed($1, $2);}
				;
				
e_top_level_decl_list
				:	top_level_decl_list {$$=$1;}
				|   {$$=nullptr;}
				;
				
top_level_decl_list
				:	top_level_decl_list top_level_decl ';' {$$=TopLevelDeclListNode::addElemToList($1, $2);}
				|	top_level_decl ';' {$$=TopLevelDeclListNode::createList($1);}
				;
				
top_level_decl	:	decl {$$=TopLevelDeclNode::createTopLevelDecl($1);}
				|	func_decl {$$=TopLevelDeclNode::createTopLevelDecl($1);}
				;
				
func_decl		:	FUNC ID signature {$$=FuncDeclNode::createFuncDecl($2, $3, nullptr);}
				|	FUNC ID signature block {$$=FuncDeclNode::createFuncDecl($2, $3, $4);}
				;

package_clause	:	PACKAGE ID {$$=PackageClauseNode::createNode($2);}
				;

e_stmt_list     :   stmt_list {$$=$1;}
                |   {$$=nullptr;}
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
                |   {$$=nullptr;}
                ;
				
simple_stmt		:	expr {$$=SimpleStmtNode::createExpr($1);}
				|	expr INC {$$=SimpleStmtNode::createInc($1);}
				|	expr DEC {$$=SimpleStmtNode::createDec($1);}
				|	expr_list '=' expr_list {$$=SimpleStmtNode::createAssign($1, $3);}
				|	expr_list WALRUS expr_list {$$=SimpleStmtNode::createShortVarDecl($1, $3);}
				;
				
return_stmt		:	RETURN {$$=StmtNode::createReturn(nullptr);}
				|	RETURN expr_list {$$=StmtNode::createReturn($2);}
				;
				
block			:	'{' e_stmt_list '}' {$$=StmtNode::createBlock($2);}
				;

decl			:	CONST const_spec {$$=DeclNode::createDecl($2);}
                |	CONST '(' const_spec_list ')' {$$=DeclNode::createDecl($3);}
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
				|	FOR expr block {$$=StmtNode::createFor($1, $2);}
				|	FOR e_simple_stmt ';' e_expr ';' e_simple_stmt block {$$=StmtNode::createFor($2, $4, $6, $7);}
				|	FOR RANGE expr block {$$=StmtNode::createFor(nullptr, $3, $4);}
				|	FOR expr_list '=' RANGE expr block {$$=StmtNode::createFor($2, $5, $6);}
				|	FOR expr_list WALRUS RANGE expr block {$$=StmtNode::createFor($2, $5, $6);}
				;

e_expr_case_clause_list
				:	expr_case_clause_list {$$=$1;}
				|   {$$=nullptr;}
				;

expr_case_clause_list
				:	expr_case_clause_list expr_case_clause {$$=CaseListNode::addCaseToList($1, $2);}
				|	expr_case_clause {$$=CaseListNode::createCaseList($1);}
				;
				
expr_case_clause:	expr_switch_case ':' stmt_list {$$=CaseNode::createCase($1, $3);}
				;
					
expr_switch_case:	CASE expr_list {$$=$2;}
				|	DEFAULT {$$=nullptr;}
				;
				
const_spec_list	:	const_spec_list const_spec ';' {$$=ConstSpecListNode::addConstSpecToList($1, $2);}
				|	const_spec ';' {$$=ConstSpecListNode::createConstSpecList($1);}
				;

const_spec		:	id_list {$$=ConstSpecNode::createConstSpec($1, nullptr, nullptr);}
				|	id_list '=' expr_list {$$=ConstSpecNode::createConstSpec($1, nullptr, $3);}
				|	id_list type '=' expr_list {$$=ConstSpecNode::createConstSpec($1, $2, $4);}
				;
				
id_list			:	id_list ',' ID {$$=IdListNode::addIdToList($1, $3);}
				|	ID {$$=IdListNode::createIdList($1);}
				;
				
type			:	type_name {$$=TypeNode::createNamedType($1);}
				|	'[' expr ']' type {$$=TypeNode::createArrayType($2, $4);}
                |	FUNC signature {$$=TypeNode::createFuncType($2);}
                |	'[' ']' type {$$=TypeNode::createSliceType($3);}
				;
				
type_name		:	INT {$$=$1;}
				|	FLOAT {$$=$1;}
				|	BOOL {$$=$1;}
				|	STRING {$$=$1;}
				|	RUNE {$$=$1;}
				;
				
signature		:	'(' param_list ')' results {$$=SignatureNode::createSignature($2, $4);}
				|	'(' param_list ')' {$$=SignatureNode::createSignature($2, nullptr);}
				;
				
results			:	'(' param_list ')' {$$=ResultNode::createResult($2);}
				|	type {$$=ResultNode::createResult($1);}
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
                |   {$$=nullptr;}
                ;

expr			:	ID {$$=ExprNode::createIdentifier($1);}
				|	'(' expr ')' {$$=$2;}
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

