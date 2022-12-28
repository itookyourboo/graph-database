#ifndef LOW_LEVEL_PROGRAMMING_TASK1_ATTRIBUTE_MODEL_H
#define LOW_LEVEL_PROGRAMMING_TASK1_ATTRIBUTE_MODEL_H

#include "../mem.h"
#include "string_model.h"
#include "data_type_model.h"

typedef struct __attribute__((packed)) {
    pointer name_string_pointer;
    DataType data_type;
} DB_Attribute;


DB_Attribute *db_attribute_init();
amount db_attribute_size(DB_Attribute *db_attribute);
void db_attribute_free(DB_Attribute *db_attribute);


typedef struct __attribute__((packed)) {
    String *name;
    DataType data_type;
} Attribute;


Attribute *attribute_init();
amount attribute_size(Attribute *attribute);
void attribute_free(Attribute *attribute);
Attribute *attribute_copy(Attribute *attribute);


#endif //LOW_LEVEL_PROGRAMMING_TASK1_ATTRIBUTE_MODEL_H
