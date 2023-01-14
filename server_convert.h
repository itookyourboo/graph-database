#ifndef LOW_LEVEL_PROGRAMMING_TASK3_SERVER_CONVERT_H
#define LOW_LEVEL_PROGRAMMING_TASK3_SERVER_CONVERT_H

#include "db_lib/main.h"
#include "gen-c_glib/query_types.h"

Query convert_query(I_Query *query);
I_QueryResult *convert_result(QueryResult query_result, QueryType q_type);

#endif //LOW_LEVEL_PROGRAMMING_TASK3_SERVER_CONVERT_H
