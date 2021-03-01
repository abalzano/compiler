%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "y.tab.h"
#include "scalpa.h"

int current_line = 1;

%}
%%
\(\*([^*]|[\r\n]|(\*+([^*\)]|[\r\n])))*\*+\) (void)0;

[0-9]+ {
    struct expr_t temp;
    temp.quad_op_type = QO_CST;
    temp.type = INT;
    temp.const_int = atoi(yytext);
    yylval.expr_u = temp;
    return CTE;
}
\"(((\\\")|[^\"]))*\" {
    struct expr_t temp;
    temp.quad_op_type = QO_CST;
    temp.type = STRING;
    temp.const_string = malloc(yyleng + 1);
    MCHECK(temp.const_string);
    strncpy(temp.const_string , yytext, yyleng);
    temp.const_string[yyleng] = '\0';
    yylval.expr_u = temp;
    return CTE;
}
"true" {
    struct expr_t temp;
    temp.quad_op_type = QO_CST;
    temp.type = BOOL;
    temp.const_bool = 1;
    yylval.expr_u = temp;
    return CTE;
}
"false" {
    struct expr_t temp;
    temp.quad_op_type = QO_CST;
    temp.type = BOOL;
    temp.const_bool = 0;
    yylval.expr_u = temp;
    return CTE;
}

"("             return '(';
")"             return ')';
"["             return '[';
"]"             return ']';
","             return ',';
";"             return ';';
":"             return ':';
":="            return ASSIGNMENT;
".."            return RANGELIST_SEPARATOR;
"+"             return OPB_PLUS;
"-"             return OP_MINUS;
"*"             return OPB_STAR;
"/"             return OPB_DIVIDE;
"^"             return OPB_POW;
"<"             return OPB_L;
"<="            return OPB_L_EQ;
">"             return OPB_G;
">="            return OPB_G_EQ;
"="             return OPB_EQ;
"<>"            return OPB_DIFF;
"and"           return OPB_AND;
"or"            return OPB_OR;
"xor"           return OPB_XOR;
"not"           return OPU_NOT;
"var"           return VAR;
"of"            return OF;
"array"         return ARRAY;
"program"       return PROGRAM;
"function"      return FUNCTION;
"ref"           return REF;
"unit"          return UNIT_TYPE;
"int"           return INT_TYPE;
"bool"          return BOOL_TYPE;
"read"          return READ;
"write"         return WRITE;
"begin"         return BEGIN_;
"end"           return END;
"if"            return IF;
"then"          return THEN;
"else"          return ELSE;
"return"        return RETURN;
"while"         return WHILE;
"do"            return DO;

[a-zA-Z_][a-zA-Z_0-9]* {
    yylval.str_u = malloc(yyleng+1);
    MCHECK(yylval.str_u);
    strncpy(yylval.str_u , yytext, yyleng);
    yylval.str_u[yyleng] = '\0';
    return IDENT;
};

\n              current_line ++;
[ \t]+          (void)0;
.               {fprintf(stderr, "unknown char [%s]\n", yytext); return 0;}
%%