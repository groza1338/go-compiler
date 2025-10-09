%{ // Пролог
#include <stdio.h>
#include <stdlib.h>

using namespace std;
%}
// Секция объявлений

%token	PACKAGE
%token	CONST

%token ID


%start program

%%
// Секция правил грамматики

program			:	package_clause stat_list
				;

package_clause	:	PACKAGE  ID
				;

stmt_list		:	stmt_list stmt
				|	stmt
				;
				
stmt			:	decl 
				| 	simple_stmt 
				|	return_stmt 
				| 	break_stmt 
				| 	continue_stmt 
				| 	block 
				| 	if_stmt 
				| 	wwitc_stmt 
				| 	for_stmt
				;

decl			:	const_decl	
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
				
expr_list		:	expr_list ',' expr
				|	expr
				;

%%
// Секция пользовательского кода

