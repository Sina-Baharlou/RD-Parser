/** Pascal Compiler project - Sina Moayed Baharlou 
    Winter 2012
 **/


// token to string 

char *tokens_type_string[]=	
	{
	"identifier",
	"keyword",
	"literal",
	"operator",
	"punctuation",
	"comment",
	"unknown",
	"end of file"
	};
		
	

char*tokens_id_string[]=
	{
	/* -- Punctuations -- */
	"(",
	")",
	"[",
	"]",
	";",
	":",
	",",
	/* multi char punctuation */
	".",
	"..",
	":=",

	/* -- Operators -- */

	"+",
	"-",
	"*",
	"/",
	"=",
	"<",
	">",
	// -- multi char operators 
	"div",
	"mod",
	"and",
	"or",	//O_XOR,
	"not",  //O_SHL, //O_SHR,
	"<>",
	"<=",
	">=",

	/* -- Literals -- */
	
	"integer",
	"real",
	"string",
	"true",
	"false",
	"nil",

};


	/* -- Punctuations -- */

int tokens_type_index[]=
	{
	'(',
	')',
	'[',
	']',
	';',
	':',
	',',
	// -- multi char punctuation
	300,
	301,
	302,

	/* -- Operators -- */

	'+',
	'-',
	'*',
	'/',
	'=',
	'<',
	'>',
	// -- multi char operators
	400,
	401,
	402,
	403,	//O_XOR,
	404,  //O_SHL, //O_SHR,
	405,
	406,
	407,

	/* -- Literals -- */
	
	500,
	501,
	502,
	503,
	504,
	505};

	/* -- Keywords -- */
	

char*keywords[]={
	"downto",
	"if", 
	"then",
	"array",
	"else",
	"in",
	"packed",
	"to",
	"begin",
	"end", 
	"label", 
	"procedure", 
	"type",
	"case", 
	"file", 
	"program", 
	"until",
	"const", 
	"for", 
	"record", 
	"var",
	"function", 
	"repeat", 
	"while",
	"do", 
	"goto", 
	"of", 
	"set", 
	"with", 
	"integer", 
	"shortint",
	"longint", 
	"comp", 
	"byte", 
	"word", 
	"real", 
	"single", 
	"double"};


char*get_token_type_name(int token_id)
{
	return tokens_type_string[token_id-T_IDENTIFIER];
}

char * get_token_id_name(int token_id,int secondary_id)
{
	
	int i=0;

	switch (token_id)
	{
		case T_KEYWORD :
	
			i=secondary_id-K_DOWNTO;
		
			return keywords[i];
			
			break;
		
		case T_OPERATOR:case T_PUNCTUATION:case T_LITERAL:
	
			while(secondary_id!=tokens_type_index[i])i++;

			return tokens_id_string[i];
		
			break;
			
		case T_IDENTIFIER:
			return " "; // (char*)c_token->data;
			break;

		case T_EOF:
			return "end of file";
			break;

		case T_COMMENT:
			return "comment";
			break;

		case T_UNKNOWN:
			return "unknown";
			break;
	
	}
}




