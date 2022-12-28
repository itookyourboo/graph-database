%option noyywrap noinput nounput yylineno

%{
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>

#include "db_model.h"
#include "query.h"
#include "parser.tab.h"

void showError();

static int next_column = 1;
int column = 1;

    #define HANDLE_COLUMN column = next_column; next_column += strlen(yytext)

    char *lineptr = NULL;
    size_t i = 0;
    size_t consumed = 0;
    size_t available = 0;

    size_t min(size_t a, size_t b);
    #define YY_INPUT(buf,result,max_size) {\
        if(available <= 0) {\
            consumed = 0;\
            available = getline(&lineptr, &i, yyin);\
            if (available < 0) {\
                if (ferror(yyin)) { perror("read error:"); }\
                    available = 0;\
                }\
        }\
        result = min(available, max_size);\
        strncpy(buf, lineptr + consumed, result);\
        consumed += result;\
        available -= result;\
    }
%}

%%

";" {
    HANDLE_COLUMN;
    return COL;
}

\"[^\"]*\" {
    HANDLE_COLUMN;
    int len = strlen(yytext);
    yylval.string = malloc(len - 1);
    strncpy(yylval.string, yytext + 1, len - 2);
    yylval.string[len - 2] = '\0';
    return STRING;
}

[0-9]+\.[0-9]*e?-?[0-9]* {
    HANDLE_COLUMN;
    yylval.floating = atof(yytext);
    return FLOATING;
}

[0-9]+ {
    HANDLE_COLUMN;
    yylval.integer = atoi(yytext);
    return INTEGER;
}

"true" {
    HANDLE_COLUMN;
    yylval.boolean = true;
    return BOOLEAN;
}

"false" {
    HANDLE_COLUMN;
    yylval.boolean = false;
    return BOOLEAN;
}

"(" {
    HANDLE_COLUMN;
    return L;
}

")" {
    HANDLE_COLUMN;
    return R;
}

"," {
    HANDLE_COLUMN;
    return COMMA;
}

"." {
    HANDLE_COLUMN;
    return DOT;
}

"g" {
    HANDLE_COLUMN;
    return g;
}

"S" {
    HANDLE_COLUMN;
    return S;
}

"addS" {
    HANDLE_COLUMN;
    return addS;
}

"propertyKey" {
    HANDLE_COLUMN;
    return propertyKey;
}

"V" {
    HANDLE_COLUMN;
    return V;
}

"E" {
    HANDLE_COLUMN;
    return E;
}

"in" {
    HANDLE_COLUMN;
    yylval.link_type = LT_FROM;
    return in;
}

"out" {
    HANDLE_COLUMN;
    yylval.link_type = LT_TO;
    return out;
}

"addV" {
    HANDLE_COLUMN;
    return addV;
}

"property" {
    HANDLE_COLUMN;
    return property;
}

"has" {
    HANDLE_COLUMN;
    return has;
}

"as" {
    HANDLE_COLUMN;
    return as;
}

"addE" {
    HANDLE_COLUMN;
    return addE;
}

"both" {
    HANDLE_COLUMN;
    yylval.link_type = LT_BOTH;
    return both;
}

"from" {
    HANDLE_COLUMN;
    yylval.link_type = LT_FROM;
    return from;
}

"to" {
    HANDLE_COLUMN;
    yylval.link_type = LT_TO;
    return to;
}

"Text" {
    HANDLE_COLUMN;
    yylval.vt = VT_STRING;
    return Text;
}

"Int" {
    HANDLE_COLUMN;
    yylval.vt = VT_INTEGER;
    return Int;
}

"Float" {
    HANDLE_COLUMN;
    yylval.vt = VT_FLOATING;
    return Float;
}

"Bool" {
    HANDLE_COLUMN;
    yylval.vt = VT_BOOLEAN;
    return Bool;
}

"and" {
    HANDLE_COLUMN;
    return and;
}

"or" {
    HANDLE_COLUMN;
    return or;
}

"gt" {
    HANDLE_COLUMN;
    yylval.cmp = CMP_GT;
    return gt;
}

"lt" {
    HANDLE_COLUMN;
    yylval.cmp = CMP_LT;
    return lt;
}

"ge" {
    HANDLE_COLUMN;
    yylval.cmp = CMP_GE;
    return ge;
}

"le" {
    HANDLE_COLUMN;
    yylval.cmp = CMP_LE;
    return le;
}

"eq" {
    HANDLE_COLUMN;
    yylval.cmp = CMP_EQ;
    return eq;
}

"neq" {
    HANDLE_COLUMN;
    yylval.cmp = CMP_NEQ;
    return neq;
}

"contains" {
    HANDLE_COLUMN;
    yylval.cmp = CMP_CONTAINS;
    return contains;
}

"where" {
    HANDLE_COLUMN;
    return where;
}

"dropE" {
    HANDLE_COLUMN;
    return dropE;
}

"drop" {
    HANDLE_COLUMN;
    return drop;
}

[ \t] {
    HANDLE_COLUMN;
    ;
}

\n {
    HANDLE_COLUMN;
    next_column = 1;
}

[a-zA-Z_]+ {
    HANDLE_COLUMN;
    showError(yytext);
}

%%

void showError(char* token) {
    fprintf(stderr, "Unknown token: %s\n", token);
}

size_t min(size_t a, size_t b) {
    return b < a ? b : a;
}
