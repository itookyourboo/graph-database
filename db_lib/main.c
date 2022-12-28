#include "main.h"
#include "database/visualize.h"

QueryResult execute_db_query(Query query) {
    printf("Executing query...\n");
    if (query.type == SCHEMA_CREATE) {
        printf("Creating schema:\n");
        schema_print(query.schema_create_query.schema);
    }
    return (QueryResult) {
        .type = RESULT_NONE
    };
}
