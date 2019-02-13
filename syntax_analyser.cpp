/** Pascal Compiler project - Sina Moayed Baharlou 
    Winter 2012
 **/

// -- includes
#include "syntax_analyser.h"

syntax_analyser::syntax_analyser(lexical_analyser *analyser)
{
	l_analyser=analyser;
	c_token=NULL;
}

bool syntax_analyser::accept(int token_id,int secondary_id)
{
	if (c_token->token_id==token_id && 
		c_token->secondary_id==secondary_id)
	{
		get_next();
		return true;
	}
		return false;
	
}

bool syntax_analyser::is_equal(int secondary_id)
{
	if (c_token->secondary_id==secondary_id)
		return true;
	return false;
	
}


bool syntax_analyser::first(unsigned int*first_set)
{
	
	unsigned int*temp=first_set;

	while(*temp!=NULL){
		if (is_equal(*(temp++)))
			return true;
		}
	return false;
}


char * syntax_analyser::get_lexeme()
{
	
	memset(&lexeme,0,255);
	strncpy(lexeme,text+c_token->offset.lex_lb,c_token->offset.lex_hb-c_token->offset.lex_lb);
	return lexeme;
}

bool syntax_analyser::expect(int token_id,int secondary_id,my_set stop_set)
{
	if (accept(token_id,secondary_id))
		return true;
    
	if (stop_set.contains(c_token->secondary_id))
		add_error(c_token->offset.line_number,"%s '%s' missing before '%s'.",get_token_type_name(token_id),get_token_id_name(token_id,secondary_id),get_lexeme());
	else
	{
		add_error(c_token->offset.line_number,"%s '%s' expected but %s '%s'  is given.",get_token_type_name(token_id),get_token_id_name(token_id,secondary_id),get_token_type_name(c_token->token_id),get_lexeme());
		
		while(!stop_set.contains(c_token->secondary_id))
			get_next();
	}
	
	return false;
	
	
}

void syntax_analyser::get_next()
{
	if (c_token!=NULL)
	{
		if (c_token->data!=NULL)
			free(c_token->data);
		free(c_token);
	}
	
	do
		c_token=l_analyser->scan_text();
	while(c_token->token_id==T_COMMENT || c_token->token_id==T_UNKNOWN);

}


/* -- PROCEDURES -- */

void syntax_analyser::standard_type(my_set stop_set)
{
	/*  standard_type -> integer | real */
	if(accept(T_KEYWORD,K_INTEGER))
		return;
	if (accept(T_KEYWORD,K_REAL))
		return;
		
	// -- error recovery here 
	if (stop_set.contains(c_token->secondary_id))
		add_error(c_token->offset.line_number,"Standard type missing here before `%s`.",get_lexeme());
	else
	{
		add_error(c_token->offset.line_number,"Standard type expected here");
		while(!stop_set.contains(c_token->secondary_id))
			get_next();
	}

}


void syntax_analyser::type_array(my_set stop_set)
{
	/* 	type_array -> array[num..num] of standard_type  -  follow should be implemented */
	
	expect(T_KEYWORD,K_ARRAY,stop_set+'['+NUM+P_DDOT+NUM+']'+K_OF+STANDARD_TYPE);
	expect(T_PUNCTUATION,'[',stop_set+NUM+P_DDOT+NUM+']'+K_OF+STANDARD_TYPE);
	num(stop_set+P_DDOT+NUM+']'+K_OF+STANDARD_TYPE);
	expect(T_PUNCTUATION,P_DDOT,stop_set+NUM+']'+K_OF+STANDARD_TYPE);
	num(stop_set+']'+K_OF+STANDARD_TYPE);
	expect(T_PUNCTUATION,']',stop_set+K_OF+STANDARD_TYPE);
	expect(T_KEYWORD,K_OF,stop_set+STANDARD_TYPE);
	standard_type(stop_set);
	
}

void syntax_analyser::type(my_set stop_set)
{
	/* type - > type_array | standard_type */
	
	if (first(STANDARD_TYPE))
	{
		standard_type(stop_set);
		return;
	}
	
	if (first(TYPE_ARRAY))
	{
		type_array(stop_set);
		return;
	}
	

		
	// -- error recovery here 
	
	if (stop_set.contains(c_token->secondary_id))
		add_error(c_token->offset.line_number,"Type missing here before `%s`.",get_lexeme());
	else
	{
		add_error(c_token->offset.line_number,"Type expected here");
		while(!stop_set.contains(c_token->secondary_id))
			get_next();
	}

}






void syntax_analyser::identifier_loop(my_set stop_set)
{
	/* identifier_loop - > , id  identifier_loop |  ϵ*/ 
	
	while(accept(T_PUNCTUATION,P_COMMA))
		expect(T_IDENTIFIER,T_IDENTIFIER,stop_set+',');
		
}


void syntax_analyser::identifier_list(my_set stop_set)
{
	/* identifier_list - > id identifier_loop */
	
	expect(T_IDENTIFIER,T_IDENTIFIER,stop_set+IDENTIFIER_LOOP);	
	identifier_loop(stop_set);
	
}

void syntax_analyser::declarations(my_set stop_set)
{
	/* declarations - > var identifier_list : type ;  declarations | ϵ*/
	
	while (accept(T_KEYWORD,K_VAR))
	{
		identifier_list(stop_set+':'+TYPE+';'+K_VAR);
		expect(T_PUNCTUATION,':',stop_set+TYPE+';'+K_VAR);
		type(stop_set+';'+K_VAR);
		expect(T_PUNCTUATION,';',stop_set+K_VAR);
	}
	
}


void syntax_analyser::sign(my_set stop_set)
{
	/* sign - > + | - */
	
	if (accept(T_OPERATOR,O_ADD))return;
	if (accept(T_OPERATOR,O_SUB))return;
	
	
	// -- error recovery here
    
	if (stop_set.contains(c_token->secondary_id))
		add_error(c_token->offset.line_number,"Sign missing here before `%s`.",get_lexeme());
	else
	{
		add_error(c_token->offset.line_number,"Sign expected here");
		while(!stop_set.contains(c_token->secondary_id))
			get_next();
	}
}


void syntax_analyser::relop(my_set stop_set)
{
	/* relop - > = | <> | < | <= | > | >= */ 
	
	if (accept(T_OPERATOR,'='))return;
	if (accept(T_OPERATOR,O_NOTEQ))return;
	if (accept(T_OPERATOR,'<'))return;
	if (accept(T_OPERATOR,O_GEQ))return;
	if (accept(T_OPERATOR,'>'))return;
	if (accept(T_OPERATOR,O_LEQ))return;
	
	
	// -- error recovery here
	
	if (stop_set.contains(c_token->secondary_id))
		add_error(c_token->offset.line_number,"Relation operator missing here before `%s`.",get_lexeme());
	else
	{
		add_error(c_token->offset.line_number,"Relation operator expected here");
		while(!stop_set.contains(c_token->secondary_id))
			get_next();
	}	
}

void syntax_analyser::mulop(my_set stop_set)
{
	/* mulop - > * | / | div | mod | and */
	if (accept(T_OPERATOR,O_MUL))return;
	if (accept(T_OPERATOR,O_DIV))return; // `/`
	if (accept(T_OPERATOR,O_DIVS))return;// div
	if (accept(T_OPERATOR,O_MOD))return;
	if (accept(T_OPERATOR,O_AND))return;
	
	// -- error recovery here
	
	if (stop_set.contains(c_token->secondary_id))
		add_error(c_token->offset.line_number,"Multiplication operator missing here before `%s`.",get_lexeme());
	else
	{
		add_error(c_token->offset.line_number,"Multiplication operator expected here");
		while(!stop_set.contains(c_token->secondary_id))
			get_next();
	}
	
}

void syntax_analyser::addop(my_set stop_set)
{
	/* addop - > + | - | or */
	
	if (accept(T_OPERATOR,O_ADD))return;
	if (accept(T_OPERATOR,O_SUB))return;
	if (accept(T_OPERATOR,O_OR))return;
	
	// -- error recovery here
	
	if (stop_set.contains(c_token->secondary_id))
		add_error(c_token->offset.line_number,"Additive operator missing here before `%s`.",get_lexeme());
	else
	{
		add_error(c_token->offset.line_number,"Additive operator expected here");
		while(!stop_set.contains(c_token->secondary_id))
			get_next();
	}
}


void syntax_analyser::num(my_set stop_set)
{
	/* num - > integer | real */
	
	if (accept(T_LITERAL,L_INTEGER))return;
	if (accept(T_LITERAL,L_REAL))return;
	
	
	// -- error recovery here
	
	if (stop_set.contains(c_token->secondary_id))
		add_error(c_token->offset.line_number,"number missing here before `%s`.",get_lexeme());
	else
	{
		add_error(c_token->offset.line_number,"number expected here");
		while(!stop_set.contains(c_token->secondary_id))
			get_next();
	}
}


void syntax_analyser::term(my_set stop_set)
{
	 /* term - > factor term_loop */
	
	factor(stop_set+TERM_LOOP); 
	term_loop(stop_set);
	
}

void syntax_analyser::term_loop(my_set stop_set)
{
	/* term_loop - > mulop factor term_loop | ϵ*/
	
	while(first(MULOP))
	{
		mulop(stop_set+FACTOR+MULOP);
		factor(stop_set+MULOP);
	}
	
}

void syntax_analyser::simple_expression(my_set stop_set)
{
	/*  simple_expression - > se_static_part se_loop */
	
	se_static_part(stop_set+SE_LOOP);
	se_loop(stop_set);
	
}

void syntax_analyser::se_static_part(my_set stop_set)
{
	/* se_static_part - > term | sign term */
	if (first(TERM))
	{
		term(stop_set);
		return;
	} 
	if (first(SIGN))
	{
		sign(stop_set+TERM);
		term(stop_set);
		return;
	}
	
	// -- error recovery here
	
	if (stop_set.contains(c_token->secondary_id))
		add_error(c_token->offset.line_number,"Term missing here before `%s`.",get_lexeme());
	else
	{
		add_error(c_token->offset.line_number,"Term expected here");
		while(!stop_set.contains(c_token->secondary_id))
			get_next();
	}
	
}

void syntax_analyser::se_loop(my_set stop_set)
{

	/* se_loop - > addop term se_loop | ϵ*/

	while(first(ADDOP))
	{
		addop(stop_set+TERM+ADDOP);
		term(stop_set+ADDOP);
	}
	

}


void syntax_analyser::expression(my_set stop_set)
{
	/* expression - > simple_expression expression_relop */
	
	simple_expression(stop_set+EXPRESSION_RELOP);
	expression_relop(stop_set);
}

void syntax_analyser::expression_relop(my_set stop_set)
{
	/* expression_relop - > relop simple_expression | ϵ*/
	
	if (first(RELOP))
	{
		relop(stop_set + SIMPLE_EXPRESSION);
		simple_expression(stop_set);
	}
}


void syntax_analyser::expression_list(my_set stop_set)
{
	/* expression_list - > expression expression_list_loop */
	
	expression(stop_set+EXPRESSION_LIST_LOOP);
	expression_list_loop(stop_set);
	
}


void syntax_analyser::expression_list_loop(my_set stop_set)
{
	/*expression_list_loop - > , expression expression_list_loop |  ϵ*/
	
	while(accept(T_PUNCTUATION,P_COMMA))
		expression(stop_set+P_COMMA);	
	
}

void syntax_analyser::factor(my_set stop_set)
{
	/* factor - > id id_expression | num | (expression) | not factor */
	
	
	if (accept(T_IDENTIFIER,T_IDENTIFIER))
	{
		id_expression(stop_set);
		return;
	}
	if (first(NUM))
	{
		num(stop_set);
		return;
	}
	if (accept(T_PUNCTUATION,'('))
	{
		expression(stop_set+')');
		expect(T_PUNCTUATION,')',stop_set);
		return;
	}
	
	if (accept(T_OPERATOR,O_NOT))
	{
		factor(stop_set);
		return;
	}
	
	// -- error recovery here
	
	if (stop_set.contains(c_token->secondary_id))
		add_error(c_token->offset.line_number,"Factor missing here before `%s`.",get_lexeme());
	else
	{
		add_error(c_token->offset.line_number,"Factor type expected here");
		while(!stop_set.contains(c_token->secondary_id))
			get_next();
	}
	
}


void syntax_analyser::id_expression(my_set stop_set)
{
	/* id_expression -> ( expression_list ) |   ϵ*/
	
	if (accept(T_PUNCTUATION,'('))
	{
		expression_list(stop_set+')');
		expect(T_PUNCTUATION,')',stop_set);
		return;
	}
}

void syntax_analyser::var_expression(my_set stop_set)
{
/* var_expression -> [ expression ] assignop expression | assignop expression */
	
	if (accept(T_PUNCTUATION,'['))
	{
		expression(stop_set+']'+P_ASSIGN+EXPRESSION);
		expect(T_PUNCTUATION,']',stop_set+P_ASSIGN+EXPRESSION);
		expect(T_PUNCTUATION,P_ASSIGN,stop_set+EXPRESSION);
		expression(stop_set);
		return;
	}
	
	if (accept(T_PUNCTUATION,P_ASSIGN))
	{
		expression(stop_set);
		return;
	}
	
	/* ----------- error recovery here ------------------- */
	/*
	if (stop_set.contains(c_token->secondary_id))
		add_error(c_token->offset.line_number,"Var Expression missing here before `%s`.",get_lexeme());
	else
	{
		add_error(c_token->offset.line_number,"Var Expression type expected here");
		while(!stop_set.contains(c_token->secondary_id))
			get_next();
	}
	*/
	/* --------------------------------------------------*/
	
	
}


void syntax_analyser::parameter_list(my_set stop_set)
{
	/* parameter_list - >	identifier_list : type parameter_loop  */
	
	identifier_list(stop_set + ':' + TYPE + PARAMETER_LOOP);
	
	expect(T_PUNCTUATION,':' , stop_set + TYPE + PARAMETER_LOOP);
	
	type(stop_set + PARAMETER_LOOP);
	
	parameter_loop(stop_set);	
}



void syntax_analyser::parameter_loop(my_set stop_set)
{
	/* parameter_loop - > ; identifier_list : type parameter_loop | ϵ*/
	
	while(accept(T_PUNCTUATION,';'))
	{
		identifier_list(stop_set+':'+TYPE+';');
		expect(T_PUNCTUATION,':',stop_set+TYPE+';');
		type(stop_set+';');
	}
	
}


void syntax_analyser::arguments(my_set stop_set)
{
	/* arguments - > ( parameter_list ) | ϵ*/
	
	if (accept(T_PUNCTUATION,'('))
	{
		parameter_list(stop_set+')');
		expect(T_PUNCTUATION,')',stop_set);
	}
}


void syntax_analyser::subprogram_head(my_set stop_set)
{
	/* subprogram_head - >	function id arguments : standard_type ; 
	|  procedure id arguments ; */
	
	if (accept(T_KEYWORD,K_FUNCTION))
	{
		expect(T_IDENTIFIER,T_IDENTIFIER,stop_set+ARGUMENTS+':'+STANDARD_TYPE+';');
		arguments(stop_set+':'+STANDARD_TYPE+';');
		expect(T_PUNCTUATION,':',stop_set+STANDARD_TYPE+';');
		standard_type(stop_set+';');
		expect(T_PUNCTUATION,';',stop_set);
		
		return;
	}
	
	if (accept(T_KEYWORD,K_PROCEDURE))
	{
		expect(T_IDENTIFIER,T_IDENTIFIER,stop_set+ARGUMENTS+';');
		arguments(stop_set+';');
		expect(T_PUNCTUATION,';',stop_set);
		return;
	}

	// -- error recovery here
	
	if (stop_set.contains(c_token->secondary_id))
		add_error(c_token->offset.line_number,"Subprogram head missing here before `%s`.",get_lexeme());
	else
	{
		add_error(c_token->offset.line_number,"Subprogram head expected here");
		while(!stop_set.contains(c_token->secondary_id))
			get_next();
	}

}


void syntax_analyser::subprogram_declaration(my_set stop_set)
{
	/* subprogram_declaration - >  subprogram_head declarations compound_statement */

	subprogram_head(stop_set+DECLARATIONS+COMPOUND_STATEMENT);
	declarations(stop_set+COMPOUND_STATEMENT);
	compound_statement(stop_set);

}


void syntax_analyser::subprogram_declarations(my_set stop_set)
{
	/* subprogram_declarations - > subprogram_declaration subprogram_declarations | ϵ*/
	
	while(first(SUBPROGRAM_DECLARATIONS))
	{
		subprogram_declaration(stop_set+SUBPROGRAM_DECLARATIONS);
	}

}


void syntax_analyser::compound_statement(my_set stop_set)
{
	/* compound_statement - > begin optional_statements end */
	
	expect(T_KEYWORD,K_BEGIN,stop_set+OPTIONAL_STATEMENTS+K_END);
	optional_statements(stop_set+K_END);
	expect(T_KEYWORD,K_END,stop_set);
	
}


void syntax_analyser::optional_statements(my_set stop_set)
{
	/* optional_statements - > statement_list | ϵ*/
	
	if (first(STATEMENT_LIST))
		statement_list(stop_set+STATEMENT_LIST);
}


void syntax_analyser::statement_list(my_set stop_set)
{
	/*statement_list - > statement statement_loop */
	
	statement(stop_set+STATEMENT_LOOP);
	statement_loop(stop_set);
	
}

void syntax_analyser::statement_loop(my_set stop_set)
{
	/*statement_loop - > ; statement statement_loop | ϵ*/
	
	while(accept(T_PUNCTUATION,';'))
		statement(stop_set+';');
		
}


void syntax_analyser::if_statement(my_set stop_set)
{
	/* if_statement - > if expression then statement else_part */ 

	expect(T_KEYWORD,K_IF,stop_set+EXPRESSION+K_THEN+STATEMENT+ELSE_PART);
	expression(stop_set+K_THEN+STATEMENT+ELSE_PART);
	expect(T_KEYWORD,K_THEN,stop_set+STATEMENT+ELSE_PART);
	statement(stop_set+ELSE_PART);
	else_part(stop_set);
	
}

void syntax_analyser::else_part(my_set stop_set)
{
	/* else_part - > else statement | ϵ*/
	if (accept(T_KEYWORD,K_ELSE))
		statement(stop_set);
	
}


void syntax_analyser::while_statement(my_set stop_set)
{
	/*while_statement - > while expression do statement  */
	
	expect(T_KEYWORD,K_WHILE,stop_set + EXPRESSION + K_DO + STATEMENT);
	expression(stop_set + K_DO + STATEMENT);
	expect(T_KEYWORD,K_DO,stop_set +  STATEMENT);
	statement(stop_set);
	
}


void syntax_analyser::variable_procedure(my_set stop_set)
{
	/* variable_procedure - > var_expression | id_expression | ϵ*/ 
	
	if (first(VAR_EXPRESSION))
	{
		var_expression(stop_set/* + P_ASSIGN + EXPRESSION*/);
		/*expect(T_PUNCTUATION,P_ASSIGN,stop_set+EXPRESSION);
		expression(stop_set);*/
		return;
	}
	if (first(ID_EXPRESSION))
	{
		id_expression(stop_set);
		return;
	}
	

	// -- error recovery here
	/*
	if (stop_set.contains(c_token->secondary_id))
		add_error(c_token->offset.line_number,"procedure statement or variable expression missing here before `%s`.",get_lexeme());
	else
	{
		add_error(c_token->offset.line_number,"procedure statement or variable expression expected here");
		while(!stop_set.contains(c_token->secondary_id))
			get_next();
	}
	*/
	/* --------------------------------------------------*/
	
}

void syntax_analyser::statement(my_set stop_set)
{
	/*statement - > id variable_procedure |   
					compound_statement | if_statement | while_statement */

		
	/* old one : statement - > variable assignop expression | procedure_statement |   
					compound_statement | if_statement | while_statement */
	if (accept(T_IDENTIFIER,T_IDENTIFIER))
	{
		variable_procedure(stop_set);
		return;
	}
	
	
	if (first(COMPOUND_STATEMENT))
	{
		compound_statement(stop_set);
		return;
	}
	
	if (first(IF_STATEMENT))
	{
		if_statement(stop_set);
		return;
	}
	
	if (first(WHILE_STATEMENT))
	{
		while_statement(stop_set);
		return;
	}

	// -- error recovery here
	
	if (stop_set.contains(c_token->secondary_id))
		add_error(c_token->offset.line_number,"Statement missing here before `%s`.",get_lexeme());
	else
	{
		add_error(c_token->offset.line_number,"Statement expected here");
		while(!stop_set.contains(c_token->secondary_id))
			get_next();
	}
	
		
}


void syntax_analyser::program(my_set stop_set)
{
	/* program - > program id (identifier_list ) ; declarations subprogram_declarations compound_statement . */
	
	expect(T_KEYWORD,K_PROGRAM,stop_set+T_IDENTIFIER + '(' +IDENTIFIER_LIST+')'+
		 ';' + DECLARATIONS + SUBPROGRAM_DECLARATIONS+COMPOUND_STATEMENT + P_DOT );
	expect(T_IDENTIFIER,T_IDENTIFIER, stop_set + '(' +IDENTIFIER_LIST+')'+
		 ';' + DECLARATIONS + SUBPROGRAM_DECLARATIONS+COMPOUND_STATEMENT + P_DOT );
	expect(T_PUNCTUATION,'(',stop_set+IDENTIFIER_LIST+')'+
		 ';' + DECLARATIONS + SUBPROGRAM_DECLARATIONS+COMPOUND_STATEMENT + P_DOT );
	identifier_list(stop_set+')'+
		 ';' + DECLARATIONS + SUBPROGRAM_DECLARATIONS+COMPOUND_STATEMENT + P_DOT);
		 
	expect(T_PUNCTUATION,')',stop_set+ ';' + DECLARATIONS + SUBPROGRAM_DECLARATIONS+COMPOUND_STATEMENT + P_DOT );
	expect(T_PUNCTUATION,';',stop_set+ DECLARATIONS + SUBPROGRAM_DECLARATIONS+COMPOUND_STATEMENT + P_DOT );
	
	declarations(stop_set+SUBPROGRAM_DECLARATIONS+COMPOUND_STATEMENT + P_DOT);
	subprogram_declarations(stop_set+COMPOUND_STATEMENT + P_DOT);
	compound_statement(stop_set+ P_DOT);
	expect(T_PUNCTUATION,P_DOT,stop_set);
	
}


void syntax_analyser::pascal_start()
{
	get_next();
	program(my_set(EO_F));

}


void syntax_analyser::set_error_handler(void*err_handler,void*warning_handler)
{
	add_error=(error_handler)err_handler;
	add_warning=(error_handler)warning_handler;
}
