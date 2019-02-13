/** Pascal Compiler project - Sina Moayed Baharlou 
    Winter 2012
 **/

#include "lexical_analyser.h"

/* -- lexical analyser class method implementations -- */

		
	lexical_analyser::lexical_analyser(char*_text)
	{
		reset();
		text=_text;
		init_keywords();
	}
	
	void lexical_analyser::set_error_handler(void*err_handler,void*warning_handler)
	{
		add_error=(error_handler)err_handler;
		add_warning=(error_handler)warning_handler;
	}
	
	void lexical_analyser::reset()
	{
		line=1;
		i=0;
		block_number=0;
    
	}
	
	void lexical_analyser::init_keywords()
	{

		 char keywords[]="downto if then "
					"array else in packed to "
					"begin end label procedure type "
					"case file program until "
					"const for record var "
					"function repeat while "
					"do goto of set with integer shortint "
					"longint comp byte word real single double";
					
		 char operators[]="div mod and or not"; //xor not shl shr";
	
		 char literals[]="true false nil";
	
		// -- Single char punctuations
	
		escape_chars=" \t\r\n";
		punctuation_single="()[];,";
		operator_single="+-*/=";
		number_delimiter=" \t\n()[]{}<>;:,+-*/=."; // dot not included

		char * pch;
	
		pch = strtok (keywords," ");
	
		for(int i=K_DOWNTO;pch!=NULL;i++)
		{
		
			keywords_map[strdup(pch)]=i;
			pch = strtok (NULL, " ");
		}
	
		pch=NULL;	
		pch = strtok (operators," ");
		
		for(int i=O_DIVS;pch!=NULL;i++)
		{

			
			operators_map[strdup(pch)]=i;
			pch = strtok (NULL, " ");
		}
	
		pch=NULL;	
		pch = strtok (literals," ");
	
		for(int i=L_TRUE;pch!=NULL;i++)
		{
			literal_map[strdup(pch)]=i;
			pch = strtok (NULL, " ");
		}
	
		pch=NULL;
	
	}
	
	token_offset lexical_analyser::get_offset(int lb,int hb)
	{
		token_offset offset;
		memset(&offset,0,sizeof(token_offset));
	
		offset.line_number=line;
		offset.block_number=block_number;
		offset.lex_lb=lb;
		offset.lex_hb=hb;		
		return offset;
	}	
	
	/* --  Scan Part -- */
								
	token* lexical_analyser::scan_text()
	{
		int lex_lb=0;
		int lex_hb=0;
	
		char lexeme[MAX_ID_LENGTH];
		memset(lexeme,0,MAX_ID_LENGTH);
		while(isspace(text[i]))inc(i);
    
		if (text[i]==NULL) //  reach end of text
			return new token(T_EOF,T_EOF,get_offset());
			
			
		switch (text[i])
		{
			//-- Skip `//` comment part
			
			case '/':  
				if (text[i+1]=='/')
				{
					while (text[i]!='\n')
					{
						inc(i);
						if (text[i]==NULL) break;	
					}
					
					if (text[i])
						inc(i); // skip newline( not count it twice )	
					
					return new token(T_COMMENT,NULL,get_offset());	
				}	
				break;
				
			// --  Skip  { .* } comment part
			
			case '{': 
				while (text[i]!='}')
				{
					inc(i);
					if (text[i]==NULL)
					{
						add_error(line,ERR_COMMENT);
						return new token(T_COMMENT,NULL,get_offset());
						break;
					}
				}
				inc(i); // skip comment high band 
				return new token(T_COMMENT,NULL,get_offset());
				break;
				
			// -- Skip (* .* *) comment part 
				
			case '(': 
			
				if (text[i+1]=='*')
				{
					i+=2; // skip the comment low band 

					while (true)
					{
						if (text[i]==NULL)
						{
							add_error(line,ERR_COMMENT);
							return new token(T_COMMENT,NULL,get_offset());
							break;
						}
						
						if (text[i]=='*' && text[i+1]==')')
						{	
							i+=2; // skip the comment high band 
							break; 
						}
						
						inc(i);
						
					}
					return new token(T_COMMENT,NULL,get_offset());
				} 
				break;
				
				
			// -- Checking for <|<=|<> operators


				case '<':
					if (text[i+1]=='=')
					{
						i+=2; // skip current symbol
						return new token(T_OPERATOR,O_GEQ,get_offset(i-2,i));
					}
					else if (text[i+1]=='>')
					{
						i+=2; // skip current symbol
						return new token(T_OPERATOR,O_NOTEQ,get_offset(i-2,i));
					}
					else
					{
						i++; // skip current symbol 	
						return new token(T_OPERATOR,'<',get_offset(i-1,i));
					}

					break;
				
			// -- Cheking for >|>= operators
				
				case '>':
					if (text[i+1]=='=')
					{
						i+=2; // skip current symbol
						return new token(T_OPERATOR,O_LEQ,get_offset(i-2,i));
					}
					else
					{
						i++; // skip current symbol 	
						return new token(T_OPERATOR,'>',get_offset(i-1,i));
					}
				break;
				
			// -- Cheking for :|:= punctuations
	
				case ':':
					if (text[i+1]=='=')
					{
						i+=2; //skip current symbol
						return new token(T_PUNCTUATION,P_ASSIGN,get_offset(i-2,i));
					}
					else
					{
						i++; //skip current symbol 	
						return new token(T_PUNCTUATION,P_COLON,get_offset(i-1,i));
					}
				break;
				
			// -- Cheking for .|.. punctuations 
	
				case '.':
					if (text[i+1]=='.')
					{
						i+=2; //skip current symbol
						return new token(T_PUNCTUATION,P_DDOT,get_offset(i-2,i));
					}
					else
					{
						i++;
						return new token(T_PUNCTUATION,P_DOT,get_offset(i-1,i));
					}
				break;
				
			
			// -- Cheking for string literal 
				
						
				case '\'':
				
				i++; // skip low band
				
				lex_lb=i;
				
				while (text[i]!='\'')
				{
					inc(i);
					if (text[i]==NULL)
					{
						add_error(line, ERR_STRING);
						return new token(T_LITERAL,L_STRING,get_offset()); // exit main loop
						break;
					} 
				}
					
				inc(i); // skip string high band
				
				lex_hb=i-1;
				
				//-- 
				
				char*string_value=(char*)malloc(sizeof(char)*(lex_hb-lex_lb));
				
				strncpy(string_value,text+lex_lb,lex_hb-lex_lb);
				
				string_value[lex_hb-lex_lb]=0; // null terminated
				
				return new token(T_LITERAL,L_STRING,get_offset(lex_lb-1,lex_hb+1),(void*)string_value);	
				
				break;	
				
		}
		
	
				
		// -- Cheking for Single character punctuations `()[];:,`
				
		if (strchr(punctuation_single,text[i]))
			return new token(T_PUNCTUATION,text[i++],get_offset(i-1,i));
		
			
		// -- Cheking for Single character operators `+-*/=` 
		
		if (strchr(operator_single,text[i]))
			return new token(T_OPERATOR,text[i++],get_offset(i-1,i));
		
		/* -- Check for numberic literals /*
				
				
		/*			PASCAL GRAMMAR FOR NUMBERIC LITERALS
	
				number-> digits optional_fraction optional_exponenet
				digit -> | 0-9 |
				digits -> digit (digits)*
				optional_fraction -> . digits | nothing
				optional_exponent -> (E (+|-|nothing) digits ) | nothing
				
				implemented by method that acts like recursive descent alghorhytm
		*/
				
		bool is_float=false;
				
		if (is_digit(text[i]))
		{
			lex_lb=i;
				
			loop_if_digit(text[i],i);
				
					
			if(text[i]=='.' && text[i+1]!='.')	// optional fraction (skip if .. operator )
			{
				i++;
				
		
				is_float=true;
						
				if (is_digit(text[i]))
				{
					i++;
					loop_if_digit(text[i],i);
				}
				else	// -- error in optional fraction 
				{
					while(!is_number_delimiter(text[i]))i++;	// error recovery 
					strncpy(lexeme,text+lex_lb,lexeme_size);
					add_error(line,ERR_FRACTION,lexeme);
					return new token(T_LITERAL,L_REAL,get_offset());
							
				}  	
						
						
			}
					
					
			if(text[i]=='e') // -- optional exponent
			{
				i++;
				
				is_float=true;
				
				if (text[i]=='+' || text[i]=='-')
					i++;
						
				if (is_digit(text[i]))
				{
					i++;
					loop_if_digit(text[i],i);
				}
				else	// -- error in optional exponent
				{
					while(!is_number_delimiter(text[i]))i++;	// error recovery 
					
					strncpy(lexeme,text+lex_lb,lexeme_size);
							
					add_error(line,ERR_EXPONENT,lexeme);
							
					return new token(T_LITERAL,L_REAL,get_offset());
							
				}	
						
			}
					

			if (is_number_delimiter(text[i])) // check for number suffix
			{
						
				if (i-lex_lb>MAX_NUMBER_LENGTH) // number length exceeds maximum size 
				{
					add_warning(line,ERR_NUMBER_SIZE);
						return new token(T_LITERAL,L_INTEGER,get_offset());
				}
						
				strncpy(lexeme,text+lex_lb,i-lex_lb);
						
				if (is_float)
				{
					float *temp_number=(float*)malloc(sizeof(float));
					*temp_number=atof(lexeme);
					return new token(T_LITERAL,L_REAL,get_offset(lex_lb,i),(void*)temp_number);
				}
						
				int *temp_number=(int*)malloc(sizeof(int));
				*temp_number=atoi(lexeme);
				return new token(T_LITERAL,L_INTEGER,get_offset(lex_lb,i),(void*)temp_number);		
						
			}
			else	// -- bad number suffix 
			{
						
				while(!is_number_delimiter(text[i]))i++; // error recovery 
						
				strncpy(lexeme,text+lex_lb,lexeme_size);
				
				add_error(line,ERR_NUMBER_SUFFIX,lexeme);
					
				return new token(T_LITERAL,L_INTEGER,get_offset());
						
			}
		}			
	
		// --	valid delimiters - dot is not valid for numbers
		/* -- Check for keywords | identifiers -- */
	
			
		/* Pascal Grammar for identifiers 
			letter -> |a-zA-Z|
			digit -> |0-9|
			id -> letter(letter|digit)*
			
		implemented by method that acts like recursive descent alghorhytm
		*/
			
		if (is_letter(text[i]))
		{
 			lex_lb=i;
			loop_if_letter(text[i],i);
			lex_hb=i;
 			
 			// -- Error : identifier length exceeds maximum size 

 			if (lex_hb-lex_lb>MAX_ID_LENGTH) 
 			{
 				add_warning(line,ERR_ID_SIZE);
 				return new token(T_IDENTIFIER,T_IDENTIFIER,get_offset());
 			}	
 			
 				
 			strncpy(lexeme,text+lex_lb,lex_hb-lex_lb);
 				
 			if (keywords_map.count(lexeme)>0)
 				return new token(T_KEYWORD,keywords_map[lexeme],get_offset(lex_lb,lex_hb));
 			else if (operators_map.count(lexeme)>0)
 				return new token(T_OPERATOR,operators_map[lexeme],get_offset(lex_lb,lex_hb));
 			else if(literal_map.count(lexeme)>0)
 				return new token(T_LITERAL,literal_map[lexeme],get_offset(lex_lb,lex_hb));
			else
				return new token(T_IDENTIFIER,T_IDENTIFIER,get_offset(lex_lb,lex_hb),(void*)strdup(lexeme));
		}
			

		// -- Invalid token 
 		add_warning(line,ERR_UNKNOWN_CHAR,text[i]);
 		i++; //skip current symbol
 			
		return new token(T_UNKNOWN,T_UNKNOWN,get_offset(i-1,i));	
		
	}
