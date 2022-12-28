#ifndef LOW_LEVEL_PROGRAMMING_TASK1_SCHEMA_CRUD_H
#define LOW_LEVEL_PROGRAMMING_TASK1_SCHEMA_CRUD_H

#include "crud_util.h"

DECLARE_INSERT_FUNCTION(DB_Schema, db_schema)

RC schema_create(Schema *schema, Connection *connection);
RC schema_find(char *name, pointer *schema_pointer, DB_Schema *db_schema_found, Connection *connection);
RC schema_show(char *name, Schema *schema, Connection *connection);
RC schema_delete(char *name, Connection *connection);

Schema *schema_next(Generator *generator);
Generator *all_schemas(Connection *connection);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_SCHEMA_CRUD_H
