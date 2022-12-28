#ifndef LOW_LEVEL_PROGRAMMING_TASK1_VALUE_CRUD_H
#define LOW_LEVEL_PROGRAMMING_TASK1_VALUE_CRUD_H

#include "crud_util.h"

DECLARE_INSERT_FUNCTION(DB_Value, db_value)
DECLARE_RETRIEVE_FUNCTION(DB_Value, db_value)
DECLARE_UPDATE_FUNCTION(DB_Value, db_value)

RC value_insert(Value *value, pointer *insertion_pointer, Connection *connection);
RC value_retrieve(Value *value, Connection *connection);
RC value_update(Value *new_value, Connection *connection);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_VALUE_CRUD_H
