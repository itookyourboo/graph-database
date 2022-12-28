#include <malloc.h>
#include <string.h>
#include "schema_model.h"


DB_Schema *db_schema_init() {
    DB_Schema *db_schema = malloc(sizeof(DB_Schema));
    return db_schema;
}

amount db_schema_size(DB_Schema *db_schema) {
    return (
            sizeof(db_schema->name_string_pointer) +
            sizeof(db_schema->attr_count) +
            db_schema->attr_count * sizeof(*db_schema->attr_pointers) +
            sizeof(db_schema->schema_next_pointer)
    );
}

void db_schema_free(DB_Schema *db_schema) {
    free(db_schema->attr_pointers);
    free(db_schema);
}

Schema *schema_init() {
    Schema *schema = malloc(sizeof(Schema));
    return schema;
}

amount schema_size(Schema *schema) {
    amount attributes_size = 0;
    for (amount i = 0; i < schema->attr_count; i++)
        attributes_size += attribute_size(schema->attributes[i]);
    return (
            string_size(schema->name) +
            sizeof(schema->attr_count) +
            attributes_size
    );
}

void schema_free(Schema *schema) {
    string_free(schema->name);
    for (amount i = 0; i < schema->attr_count; i++)
        attribute_free(schema->attributes[i]);
//    free(schema);
}

Schema *schema_copy(Schema *src) {
    Schema *schema = schema_init();
    memcpy(schema, src, sizeof(Schema));
    schema->name = string_copy(src->name);
    schema->attributes = malloc(src->attr_count * sizeof(*schema->attributes));
    for (size_t i = 0; i < src->attr_count; i++)
        schema->attributes[i] = attribute_copy(src->attributes[i]);
    return schema;
}