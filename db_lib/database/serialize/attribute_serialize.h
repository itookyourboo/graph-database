#ifndef LOW_LEVEL_PROGRAMMING_TASK1_ATTRIBUTE_SERIALIZE_H
#define LOW_LEVEL_PROGRAMMING_TASK1_ATTRIBUTE_SERIALIZE_H

#include "../exceptions.h"
#include "../connection.h"
#include "../model/attribute_model.h"

RC db_attribute_to_bytes(DB_Attribute *db_attribute, byte *bytes);
RC db_attribute_from_bytes(DB_Attribute *db_attribute, byte *bytes);
RC db_attribute_to_attribute(DB_Attribute *db_attribute, Attribute *attribute, Connection *connection);
RC db_attribute_from_attribute(DB_Attribute *db_attribute, Attribute *attribute);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_ATTRIBUTE_SERIALIZE_H
