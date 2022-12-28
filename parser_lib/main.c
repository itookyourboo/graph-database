#include <stdio.h>
#include "query.h"
#include "printer.h"
#include "parser.tab.h"


Query get_query(const char *string) {
    Query query = {};
    yy_scan_string(string);
    yyparse(&query);
    yylex_destroy();
    return query;
}
