/** Pascal Compiler project - Sina Moayed Baharlou 
    Winter 2012
 **/

// -- definitions 
#ifndef LEXICAL_ANALYSER_H
#define LEXICAL_ANALYSER_H

// -- includes
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <map>

using namespace std;

/* -- Enumerations -- */

enum TOKEN_TYPE
{  	
	T_IDENTIFIER=200,
	T_KEYWORD,
	T_LITERAL,
	T_OPERATOR,
	T_PUNCTUATION,
	T_COMMENT,
	T_UNKNOWN,
	T_EOF
};

enum TOKEN_ID
{

	/* -- Punctuations -- */

	P_LPAREN ='(',
	P_RPAREN =')',
	P_LSBRACK='[',
	P_RSBRACK=']',
	P_SEMICOLON=';',
	P_COLON=':',
	P_COMMA=',',
	// -- multi char punctuation
	P_DOT=300,
	P_DDOT,
	P_ASSIGN,

	/* -- Operators -- */

	O_ADD =	'+',
	O_SUB =	'-',
	O_MUL = '*',
	O_DIV = '/',
	O_EQ = '=',
	O_GT='<',
	O_LT='>',
	// -- multi char operators
	O_DIVS=400,
	O_MOD,
	O_AND,
	O_OR,	//O_XOR,
	O_NOT,  //O_SHL, //O_SHR,
	O_NOTEQ,
	O_GEQ,
	O_LEQ,

	/* -- Literals -- */
	
	L_INTEGER=500,
	L_REAL,
	L_STRING,
	L_TRUE,
	L_FALSE,
	L_NILL,

	/* -- Keywords -- */

	K_DOWNTO=600,
	K_IF,
	K_THEN,
	K_ARRAY,
	K_ELSE,
	K_IN,
	K_PACKED,
	K_TO,
	K_BEGIN,
	K_END,
	K_LABEL,
	K_PROCEDURE,
	K_TYPE,	
	K_CASE,
	K_FILE,
	K_PROGRAM,
	K_UNTIL,
	K_CONST,
	K_FOR,
	K_RECORD,
	K_VAR,
	K_FUNCTION,
	K_REPEAT,
	K_WHILE,
	K_DO,
	K_GOTO,
	K_OF,
	K_SET,
	K_WITH,
	K_INTEGER,
	K_SHORTINT,
	K_LONGINT,
	K_COMP,
	K_BYTE,
	K_WORD,
	K_REAL,
	K_SINGLE,
	K_DOUBLE

};


/* -- Prototypes -- */

struct token_offset
{
	 unsigned int line_number;
	 unsigned int block_number;
	 unsigned int lex_lb;
	 unsigned int lex_hb;
};

class token
{

public :

	// -- token fields
	unsigned int token_id;
	unsigned int secondary_id;
	token_offset offset;
	void *data;
	
	
	//--token constructors
	token( unsigned int _token_id,unsigned int _secondary_id,token_offset _offset):
		 token_id(_token_id),secondary_id(_secondary_id),offset(_offset),data(NULL)
		 {}
		 
	token( unsigned int _token_id,unsigned int _secondary_id,token_offset _offset,void*_data):
	 token_id(_token_id),secondary_id(_secondary_id),offset(_offset),data(_data)
	 {}

};

/* -- Lexical analyser class prototypes -- */
	
class lexical_analyser
{
	
	private:
	
	struct cmp_str 
	{
		bool operator()  (const char *a,const  char *b) const  
		{
			return strcmp(a, b) < 0;
		}
	};
	
	map<const char*,int,cmp_str> keywords_map;
	map<const char*,int,cmp_str> operators_map;
	map<const char*,int,cmp_str> literal_map;
		
	int line;
	int block_number;
	int i;
	char*text;
	
	typedef void(*error_handler)(int line,char*message,...);

	error_handler add_error;
	error_handler add_warning;

	/* -- Keywords and multi char operators -- */
	
	char *keywords;
	char *operators;
	char *literals;
	
	/* -- single char punctuations -- */
	
	char *escape_chars;
	char *punctuation_single;
	char *operator_single;
	char *number_delimiter; // dot not included
	
	/* -- Definitions -- */
	
	#define MAX_ID_LENGTH 255
	#define MAX_NUMBER_LENGTH 16

	#define is_space(x) strchr(escape_chars,x)
	#define is_letter(x) ( (x>= 'a' && x<='z') || (x>= 'A' && x<='Z') )
	#define is_letter_ex(x) (( (x>= 'a' && x<='z') || (x>= 'A' && x<='Z') ) || (x>='0' && x<='9')) 
	#define is_digit(x) (x>='0' && x<='9')
	#define loop_if_digit(x,i) while(is_digit(x))i++
	#define loop_if_letter(x,i) while(is_letter_ex(x))i++
	#define is_number_delimiter(x) strchr(number_delimiter,x)
	#define inc(x) {if(text[x++]=='\n')line++;}
	#define lexeme_size (i-lex_lb>MAX_ID_LENGTH)?MAX_ID_LENGTH:i-lex_lb
	
	// -- Lexical errors
	#define ERR_COMMENT		"unterminated comment"
	#define ERR_STRING		"missing terminating \" character"
	#define ERR_FRACTION		"number expected after fraction symbol `%s`"
	#define ERR_EXPONENT		"number expected after exponent symbol `%s`"
	#define ERR_NUMBER_SIZE 	"number length exceeds maximum size"
	#define ERR_NUMBER_SUFFIX 	"invalid suffix for numberic token `%s`"
	#define ERR_ID_SIZE 		"identifier length exceeds maximum size"
	#define ERR_UNKNOWN_CHAR 	"unknown character `%c` in program"
	

	// -- class methods
	public:
	
	lexical_analyser(char*_text);
	
	void set_error_handler(void*err_handler,void*warning_handler);
	
	void reset();
	
	void init_keywords();
	
	token_offset get_offset(int lb=0,int hb=0);	

    token* scan_text();

};
#endif 
