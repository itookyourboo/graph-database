#include "main.h"

static char *db_name = "database.qdb";

Connection *connect() {
    Connection *connection = malloc(sizeof(Connection));
    database_open(db_name, connection);
    return connection;
}

QueryResult execute_db_query(Query query, Connection *connection) {
    return query_execute(query, connection);
}

void close_connection(Connection *connection) {
    database_close(connection);
    free(connection);
}
