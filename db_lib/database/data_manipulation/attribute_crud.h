#ifndef LOW_LEVEL_PROGRAMMING_TASK1_ATTRIBUTE_CRUD_H
#define LOW_LEVEL_PROGRAMMING_TASK1_ATTRIBUTE_CRUD_H

#include "crud_util.h"

DECLARE_INSERT_FUNCTION(DB_Attribute, db_attribute)
DECLARE_RETRIEVE_FUNCTION(DB_Attribute, db_attribute)

RC attribute_insert(Attribute *attribute, pointer *insertion_pointer, Connection *connection);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_ATTRIBUTE_CRUD_H
