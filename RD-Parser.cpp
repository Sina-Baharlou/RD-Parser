/** Pascal Compiler project - Sina Moayed Baharlou 
    Winter 2012
 **/

// -- includes 
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// -- modules
#include "lexical_analyser.h"
#include "lexical_analyser.cpp"
#include "token_to_string.cpp"
#include "syntax_analyser.h"
#include "syntax_analyser.cpp"

// -- global vars
int error_count=0;
int warning_count=0;
int size;

void add_error(int line,char *message,...)
{
	char error_msg[512];
	va_list args;
    // -- get the argument values
	va_start(args,message);
	memset(error_msg,0,512);
    // -- format the parmaters
	vsprintf(error_msg,message,args);
    // -- print the output
	printf("error - line %d : %s\r\n",line,error_msg);
	va_end(args);
	error_count++;
}

void add_warning(int line,char *message,...)
{
	char error_msg[512];
	va_list args;
    // -- get the argument values
	va_start(args,message);
	memset(error_msg,0,512);
    // -- format the parmaters
	vsprintf(error_msg,message,args);
    // -- print the output
	printf("warning - line %d : %s\r\n",line,error_msg);
	va_end(args);
	warning_count++;
}

// -- the code for main

int main(int argc,char*argv[])
{

	printf("** Pascal Compiler project - Sina Moayed Baharlou **"
		"\r\n  Winter 2012\r\n\r\n");
	if (argc<2)
	{
		printf("No pascal file were given.\r\n",argv[0]);
		return -1;
	}

	// -- open the given file
	FILE*file=fopen(argv[1],"r");
	if (file==NULL)
	{
		perror("error occurred while opening the file ");
		return -1;
	}
	// -- get the file size
	fseek(file,0,SEEK_END);
    size=ftell(file);
	fseek(file,0,SEEK_SET);

    // -- allocate the memory and read the file
	char *buffer=(char*)malloc(size);
	fread(buffer,sizeof(char),size,file);
	fclose(file);
	buffer[size]=NULL;
	// --

    // -- instanciate the lexical analyser class
	lexical_analyser *l_analyser=new lexical_analyser(buffer);
	l_analyser->set_error_handler(add_error,add_warning);
    
    // -- instanciate the syntax analyser class
	syntax_analyser* s_analyser=new syntax_analyser(l_analyser);
	s_analyser->text=buffer;
	// -- set error handlers 
    s_analyser->set_error_handler(add_error,add_warning);
	// -- perform the analysis
    s_analyser->pascal_start();

	// -- output the results
	printf("\r\nError : %d - Warning : %d\r\n",error_count,warning_count);	
	return 0;

}
