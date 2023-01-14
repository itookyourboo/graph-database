#ifndef LOW_LEVEL_PROGRAMMING_TASK3_MAIN_H
#define LOW_LEVEL_PROGRAMMING_TASK3_MAIN_H

#include "database/mem.h"
#include "database/database_manager.h"
#include "database/query_manager.h"

Connection *connect();
QueryResult execute_db_query(Query query, Connection *connection);
void close_connection(Connection *connection);

#endif //LOW_LEVEL_PROGRAMMING_TASK3_MAIN_H
