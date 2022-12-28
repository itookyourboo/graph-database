#ifndef LOW_LEVEL_PROGRAMMING_TASK1_SCHEMA_MODEL_H
#define LOW_LEVEL_PROGRAMMING_TASK1_SCHEMA_MODEL_H

#include "../mem.h"
#include "string_model.h"
#include "attribute_model.h"

typedef struct __attribute__((packed)) {
    pointer name_string_pointer;
    amount attr_count;
    pointer *attr_pointers;
    pointer schema_next_pointer;
} DB_Schema;

DB_Schema *db_schema_init();
amount db_schema_size(DB_Schema *db_schema);
void db_schema_free(DB_Schema *db_schema);

typedef struct {
    String *name;
    amount attr_count;
    Attribute **attributes;

    pointer _db_ptr;
} Schema;

Schema *schema_init();
amount schema_size(Schema *schema);
void schema_free(Schema *schema);
Schema *schema_copy(Schema *schema);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_SCHEMA_MODEL_H
