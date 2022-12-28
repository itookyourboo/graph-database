#ifndef LOW_LEVEL_PROGRAMMING_TASK1_VALUE_SERIALIZE_H
#define LOW_LEVEL_PROGRAMMING_TASK1_VALUE_SERIALIZE_H

#include "../exceptions.h"
#include "../model/value_model.h"
#include "../mem.h"
#include "../connection.h"

RC db_value_to_bytes(DB_Value *db_value, byte *bytes);
RC db_value_from_bytes(DB_Value *db_value, byte *bytes);

RC db_value_to_value(DB_Value *db_value, Value *value, Connection *connection);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_VALUE_SERIALIZE_H
