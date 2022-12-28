#ifndef LOW_LEVEL_PROGRAMMING_TASK1_VALUE_MODEL_H
#define LOW_LEVEL_PROGRAMMING_TASK1_VALUE_MODEL_H

#include "../mem.h"
#include "data_type_model.h"
#include "../comparation.h"


typedef struct __attribute__((packed)) {
    DataType data_type;
    union {
        dt_integer integer;
        pointer string_pointer;
        dt_floating floating;
        dt_boolean boolean;
    };
} DB_Value;

DB_Value *db_value_init();
amount db_value_size(DB_Value *value);
void db_value_free(DB_Value *value);

typedef struct __attribute__((packed)) {
    DataType data_type;
    union {
        dt_integer integer;
        dt_string string;
        dt_floating floating;
        dt_boolean boolean;
    };
} Value;

Value *value_init();
amount value_size(Value *value);
void value_free(Value *value);
CMP value_compare(Value *value1, Value *value2);

Value *value_copy(Value *src);

#endif //LOEW_LEVEL_PROGRAMMING_TASK1_VALUE_MODEL_H
