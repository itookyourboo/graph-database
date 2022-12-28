#ifndef LOW_LEVEL_PROGRAMMING_TASK1_SCHEMA_SERIALIZE_H
#define LOW_LEVEL_PROGRAMMING_TASK1_SCHEMA_SERIALIZE_H

#include "../exceptions.h"
#include "../model/schema_model.h"
#include "../connection.h"

RC db_schema_to_bytes(DB_Schema *db_schema, byte *bytes);
RC db_schema_from_bytes(DB_Schema *db_schema, byte *bytes);
RC db_schema_to_schema(DB_Schema *db_schema, Schema *schema, Connection *connection);
RC db_schema_from_schema(DB_Schema *db_schema, Schema *schema);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_SCHEMA_SERIALIZE_H
