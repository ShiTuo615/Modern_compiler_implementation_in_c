%{
#include <string.h>
#include "util.h"
#include "tokens.h"
#include "errormsg.h"

int charPos=1;

int yywrap(void)
{
 charPos=1;
 return 1;
}


void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}

int commentNesting = 0;

/* String buffer */
const int INITIAL_BUFFER_LENGTH = 32;
char *string_buffer;
unsigned int string_buffer_capacity;
unsigned int STRING_START = 0;

void init_string_buffer(void){
    string_buffer = checked_malloc(INITIAL_BUFFER_LENGTH);
    string_buffer[0] = 0;
    string_buffer_capacity = INITIAL_BUFFER_LENGTH;
}

void append_char_to_stringbuffer(char ch){
    size_t new_length = strlen(string_buffer) + 1;
    if(new_length == string_buffer_capacity){
        char * temp;
        string_buffer_capacity *=2;
        temp = checked_malloc(string_buffer_capacity);
        memccpy(temp, string_buffer, new_length);
        free(string_buffer);
        string_buffer = temp;
    }
    string_buffer[new_length - 1] = ch;
    string_buffer[new_length] = 0;
}

%}

%%
" "	 {adjust(); continue;}
[ \r\t] {adjust(); continue;}
\n	 {adjust(); EM_newline(); continue;}

/* Reserved words. */
array       {adjust(); return ARRAY;}
if          {adjust(); return IF;}
then        {adjust(); return THEN;}
else        {adjust(); return ELSE;}
while       {adjust(); return WHILE;}
for         {adjust(); return FOR;}
to          {adjust(); return TO;}
do          {adjust(); return DO;}
let         {adjust(); return LET;}
in          {adjust(); return IN;}
end         {adjust(); return END;}
of          {adjust(); return OF;}
break       {adjust(); return BREAK;}
nil         {adjust(); return NIL;}
function    {adjust(); return FUNCTION;}
var         {adjust(); return VAR;}
type        {adjust(); return TYPE;}

/* punctuation symbols */
","         {adjust(); return COMMA;}
":"         {adjust(); return COLON;}
";"         {adjust(); return SEMICOLON;}
"("         {adjust(); return LPAREN;}
")"         {adjust(); return RPAREN;}
"["         {adjust(); return LBRACK;}
"]"         {adjust(); return RBRACK;}
"{"         {adjust(); return LBRACE;}
"}"         {adjust(); return RBRACE;}
"."         {adjust(); return DOT;}
"+"         {adjust(); return PLUS;}
"-"         {adjust(); return MINUS;}
"*"         {adjust(); return TIMES;}
"/"         {adjust(); return DIVIDE;}
"="         {adjust(); return EQ;}
"<>"        {adjust(); return NEQ;}
"<"         {adjust(); return LT;}
"<="        {adjust(); return LE;}
">"         {adjust(); return GT;}
">="        {adjust(); return GE;}
"&"         {adjust(); return AND;}
"|"         {adjust(); return OR;}
":="        {adjust(); return ASSIGN;}

/* identifiers */
[a-z][A-Z]+[a-z|A-Z|0-9|_]* {adjust(); yylval.sval = yytext; return ID:}

/* Integer literals. */
[0-9]+	 {adjust(); yylval.ival=atoi(yytext); return INT;}

/* String literals. */
\" {adjust(); init_string_buffer(); STRING_START = charPos - 1; BEGIN(STRING_STATE);}
.	 {adjust(); EM_error(EM_tokPos,"illegal token");}


