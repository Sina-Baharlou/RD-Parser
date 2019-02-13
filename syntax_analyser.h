/** Pascal Compiler project - Sina Moayed Baharlou 
    Winter 2012
 **/

// -- definitions
#ifndef SYNTAX_ANALYSER_H
#define SYNTAX_ANALYSER_H

// -- includes 

#include <stdio.h>
#include <string.h>
#include <map>
#include <set>

// -- define new operators on class 'set'
class my_set:public set<unsigned int>
{

	
	public:
	my_set(unsigned int*set_array)
	{
		unsigned int * hb=set_array;
		while(*(++hb)!=NULL);
		insert(set_array,hb);
	}
	
	
	my_set operator +(unsigned int second_set) 
	{
		insert(second_set);
		return *this;
	}
	
	my_set operator +(unsigned int* second_set) 
	{
		unsigned int * hb=second_set;
		while(*(++hb)!=NULL);
		insert(second_set,hb);
		return *this;
	}
	
	my_set operator +(my_set second_set) 
	{
			
		insert(second_set.begin(),second_set.end());
		return *this;
	}
	
	
	bool contains(int value)
	{
		return (count(value)>0);
	}
	
	
};


unsigned int 
	PROGRAM[]={K_PROGRAM,NULL},
	STATEMENT[]={T_IDENTIFIER,K_BEGIN,K_IF,K_WHILE,NULL},
	VARIABLE_PROCEDURE[]={ '[','('/*,ϵ*/ ,NULL},
	WHILE_STATEMENT[]={ K_WHILE ,NULL}, 
	IF_STATEMENT[] ={ K_IF,NULL},
	ELSE_PART[]={ K_ELSE /*,ϵ*/,NULL},
	STATEMENT_LOOP[]= { ';' /* , ϵ*/ ,NULL},
	STATEMENT_LIST[]={T_IDENTIFIER,K_BEGIN,K_IF,K_WHILE,NULL},
	OPTIONAL_STATEMENTS[]={T_IDENTIFIER,K_BEGIN,K_IF,K_WHILE /*, ϵ*/ ,NULL},
	COMPOUND_STATEMENT[]={K_BEGIN,NULL}, 
	SUBPROGRAM_DECLARATIONS[]={ K_FUNCTION,K_PROCEDURE ,NULL},
	SUBPROGRAM_DECLARATION[] ={ K_FUNCTION,K_PROCEDURE ,NULL},
	SUBPROGRAM_HEAD[] ={ K_FUNCTION,K_PROCEDURE,NULL},
	PARAMETER_LIST[]={T_IDENTIFIER,NULL},
	PARAMETER_LOOP[]={ ';'/*,ϵ*/,NULL},
	ARGUMENTS[]={'('/*,ϵ*/,NULL},
	VAR_EXPRESSION[]={ '[',P_ASSIGN/*,ϵ*/,NULL},
	/*
	unsed :
	procedure_statement : id 
	variable : id 
	----------
	*/
	FACTOR[] ={ T_IDENTIFIER,L_INTEGER,L_REAL,'(',O_NOT ,NULL},
	ID_EXPRESSION[] ={  '('/*,ϵ*/ ,NULL},
	EXPRESSION_LIST[]= { T_IDENTIFIER,L_INTEGER,L_REAL,'(',O_NOT,O_ADD,O_SUB,NULL},
	EXPRESSION_LIST_LOOP[]= {  ','/*,ϵ*/ ,NULL},
	EXPRESSION[]={T_IDENTIFIER,L_INTEGER,L_REAL,'(',O_NOT,O_ADD,O_SUB,NULL},
	EXPRESSION_RELOP[]= {  '=',O_NOTEQ, '<' , O_GEQ , '>' , O_LEQ ,NULL},
	SIMPLE_EXPRESSION[]= { T_IDENTIFIER,L_INTEGER,L_REAL,'(',O_NOT,O_ADD,O_SUB,NULL},
	SE_STATIC_PART []={T_IDENTIFIER,L_INTEGER,L_REAL,'(',O_NOT,O_ADD,O_SUB,NULL},
	SE_LOOP[]={ O_ADD,O_SUB,O_OR,NULL},
	TERM[]={T_IDENTIFIER,L_INTEGER,L_REAL,'(',O_NOT ,NULL},
	TERM_LOOP []={ O_MUL,O_DIV,O_DIVS,O_MOD,O_AND/*,ϵ*/,NULL},
	ADDOP[]={O_ADD,O_SUB,O_OR,NULL},
	MULOP[]= { O_MUL,O_DIV,O_DIVS,O_MOD,O_AND,NULL},
	RELOP[] ={'=',O_NOTEQ, '<' , O_GEQ , '>' , O_LEQ,NULL},
	SIGN[]= { O_ADD,O_SUB ,NULL},
	NUM[]={L_INTEGER,L_REAL,NULL},
	DECLARATIONS[] ={ K_VAR/*,ϵ*/ ,NULL},
	IDENTIFIER_LIST[] ={T_IDENTIFIER,NULL},
	IDENTIFIER_LOOP[] ={','/*,ϵ*/,NULL},
	TYPE[]={K_ARRAY,K_INTEGER,K_REAL,NULL},
	TYPE_ARRAY[]={K_ARRAY,NULL},
	STANDARD_TYPE[]={K_INTEGER,K_REAL,NULL},
	EO_F[]={T_EOF,NULL};

class syntax_analyser
{

private:

	lexical_analyser *l_analyser;
	token *c_token;
	char lexeme[256];

	typedef void(*error_handler)(int line,char*message,...);

	error_handler add_error;
	error_handler add_warning;

	// -- Prototypes
	void program(my_set);
	void statement(my_set);
	void variable_procedure(my_set);
	void while_statement(my_set);
	void else_part(my_set);
	void if_statement(my_set);
	//----
	void statement_loop(my_set);
	void statement_list(my_set);
	void optional_statements(my_set);
	void compound_statement(my_set);
	//----
	void subprogram_declarations(my_set);
	void subprogram_declaration(my_set);
	void subprogram_head(my_set);
	//----
	void arguments(my_set);
	void parameter_loop(my_set);
	void parameter_list(my_set);
	///----
	void var_expression(my_set);
	void id_expression(my_set );
	//----
	void factor(my_set);
	void expression_list_loop(my_set);
	void expression_list(my_set);
	//----
	void expression_relop(my_set);
	void expression(my_set );
	//---
	void se_loop(my_set);
	void se_static_part(my_set);
		//--
	void simple_expression(my_set);
	void term_loop(my_set);
	void term(my_set);
	//--
	void addop(my_set);
	void mulop(my_set);
	void num(my_set);
	//---
	void relop(my_set);
	void sign(my_set);
	//--
	void declarations(my_set);
	void identifier_list(my_set);
	void identifier_loop(my_set);
	void type(my_set);
	void type_array(my_set);
	void standard_type(my_set);
	//--
	void get_next();
	bool expect(int token_id,int secondary_id,my_set stop_set);
	char * get_lexeme();
	bool first(unsigned int*first_set);
	bool is_equal(int secondary_id);
	bool accept(int token_id,int secondary_id);
	
	// -- Class methods
	public:
	char*text;
	syntax_analyser(lexical_analyser*);
	void set_error_handler(void*err_handler,void*warning_handler);
	void pascal_start();

};

#endif 
