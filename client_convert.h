#ifndef LOW_LEVEL_PROGRAMMING_TASK3_CLIENT_CONVERT_H
#define LOW_LEVEL_PROGRAMMING_TASK3_CLIENT_CONVERT_H

#include "gen-c_glib/query_types.h"
#include "parser_lib/query.h"

I_Query *convert_query(Query *query);
void print_result(I_QueryResult *result);

#endif //LOW_LEVEL_PROGRAMMING_TASK3_CLIENT_CONVERT_H
