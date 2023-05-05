/************************************************************************
University of Bradford

*************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"


// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE

FILE* f;
char* Gfile_name;
int l_no;
const char* Keywords[21] = {"class", "constructor", "function", "method",
                          "field", "static", "var", "int", "char", "do",
                          "boolean", "void", "true", "false", "null",
                          "this", "let", "if", "else", "while", "return"}; //an array of Keywords

const char Symbols[20] = {'{','}','(',')','[',']','.',',',';','+','-','*','/','&',',','<','>','=','~','|'};

int isKeyword(char* a)
{
  int i;
  for (i = 0; i < 21; i++)
  {
    if (strcmp(a, Keywords[i]) == 0)
    {
      return 1; //keyword found
    }
  }
  return 0; //not a keyword
}

int isSymbol(char b)
{
  int i;
  for (i = 0; i < 20; i++)
  {
    if (Symbols[i] == b)
    {
      return 1; //symbol found
    }
  }
  return 0; //not a symbol
}




// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
//Take care of all the Global variables here and reset them.
int InitLexer (char* file_name)
{
  f = fopen(file_name, "r");
  if (f == NULL)
  {
    printf("Error:can't open file\n");
    return 0;
  }
  l_no = 1;
  Gfile_name = file_name;
  return 1; //success

}



// Get the next token from the source file
Token GetNextToken ()
{
  Token t;
  strcpy(t.fl, Gfile_name);
  int c; 
  c = getc(f);
  loop:
  while(c== ' ' || c == '\t')
  c = getc(f);
  if (c == EOF)
  {
    t.tp = EOFile;
    strcpy(t.lx, "End of file");
    t.ln = l_no;
    return t;
  }
  if(c == '\r')
  c = getc(f);
  if ( c == '\n'){
    l_no++;
    c = getc(f);
    goto loop;
  }
  if (c == '/')//Two types of comments 
  {
    c = getc(f);
    if(c == '*')//Multiline comment
    {
      while(1){
        c=getc(f);
        if(c=='\n')
        l_no++;
        else if(c==EOF){
          t.tp = ERR;
          t.ec = EofInCom;
          strcpy(t.lx, "Error: unexpected eof in comment");
          t.ln = l_no;
          return t;
        }
        else if(c=='*'){
        c=getc(f);
        if(c =='/'){
          c=getc(f);
          goto loop;
        }
        }
      }
      
    }
  else if(c == '/')
    {
      while(c != '\n')
      c = getc(f);
      goto loop;
    }
  else{   //Or it is just a symbol.
    t.tp = SYMBOL;
    t.lx[0] = '/';
    t.lx[1] = '\0';
    t.ln = l_no;
    ungetc(c, f);
    return t;
  }
  }
   if (isSymbol(c) == 1)
    {
      t.tp = SYMBOL;
      t.lx[0] = c;
      t.lx[1] = '\0';
      t.ln = l_no;
      return t;
    }
  if (c == '"')
  {
    int i=0;
    while ((c = getc(f)) != EOF)
    {
      if(c == '\n')
      {
        t.tp = ERR;
        strcpy(t.lx, "Error: new line in string constant");
        t.ec = NewLnInStr;
        t.ln = l_no;
        return t;
      }
      else if(c == '"')
      {
        t.tp = STRING;
        t.ln = l_no;
        t.lx[i] = '\0';
        return t;
      }
      else
      t.lx[i++] = c;
    }
    t.tp = ERR;
    t.ec = EofInStr;
    strcpy(t.lx, "Error: unexpected eof in string constant");
    t.ln = l_no;
    return t;
  }

  int i = 0;

  if (isalpha(c)|| c == '_')
  {
    while (isalnum(c) || c == '_')
    {
      t.lx[i++] = c;
      c = getc(f);
    }
  ungetc(c, f);
  
    t.lx[i] = '\0';

    if (isKeyword(t.lx) == 1)
    {
      t.tp = RESWORD;
      t.ln = l_no;
      return t;
    }

    else
    {
       t.tp = ID;
       t.ln = l_no;
       return t;
     }
  }
  else if (isdigit(c))
  {
    i = 0;
    while (isdigit(c))
    {
      t.lx[i++] = c;
      c = getc(f);
    }
      t.lx[i]='\0';
      t.tp = INT;
      t.ln = l_no;
      ungetc(c, f);
      return t;

  }

  else
  {

    //it is an illegal symbol

      t.lx[0] = c;
      t.lx[1] = '\0';
      t.tp = ERR;
      strcpy(t.lx, "Error: illegal symbol in source file");
      t.ln = l_no;
      return t;
    
  }

}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken ()
{
  Token t = GetNextToken();
  ungetc(32, f);
  if(t.tp == STRING){
    ungetc(34,f);
    for(int i=strlen(t.lx)-1;i>=0;i--)
    ungetc(t.lx[i], f);
    ungetc(34, f);
  }
  else if(t.tp != EOFile)
    for(int i=strlen(t.lx)-1;i>=0;i--)
    ungetc(t.lx[i], f); 


  
   return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer ()
{
  fclose(f);
  l_no = 1;
	return 0;
}
// do not remove the next line
#ifndef TEST
int main ()
{
	// implement your main function here
  // NOTE: the autograder will not use your main function
  int r = InitLexer(file_name);
  if (r == 0)
  {
    return 0;
  }
  Token t;
  do
  {
    token = GetNextToken();
    if (t.tp == ERR)
    {
      printf("Error at line %d in file %s: %s\n", t.ln, t.fl, t.lx);
      break;
    }
    printf("%s\n", t.lx);
  } while (t.tp != EOFile);

}
// do not remove the next line
#endif
