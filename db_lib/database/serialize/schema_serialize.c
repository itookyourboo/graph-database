#include "schema_serialize.h"

#include "util.h"
#include "string_serialize.h"
#include "../model/schema_model.h"
#include "block_serialize.h"
#include "attribute_serialize.h"

RC db_schema_to_bytes(DB_Schema *db_schema, byte *bytes) {
    amount i = 0;
    field_to_bytes(db_schema, name_string_pointer, bytes, i);
    field_to_bytes(db_schema, attr_count, bytes, i);
    field_array_to_bytes(db_schema, attr_pointers, db_schema->attr_count, bytes, i);
    field_to_bytes(db_schema, schema_next_pointer, bytes, i);
    return RC_OK;
}

RC db_schema_from_bytes(DB_Schema *db_schema, byte *bytes) {
    amount i = 0;
    field_from_bytes(db_schema, name_string_pointer, bytes, i);
    field_from_bytes(db_schema, attr_count, bytes, i);
    field_array_from_bytes(db_schema, attr_pointers, db_schema->attr_count, bytes, i);
    field_from_bytes(db_schema, schema_next_pointer, bytes, i);
    return RC_OK;
}

RC db_schema_to_schema(DB_Schema *db_schema, Schema *schema, Connection *connection) {
    connection_move_to_pointer(db_schema->name_string_pointer, connection);
    Block *block_string_name = block_init();
    block_from_file(block_string_name, connection);
    schema->name = string_init();
    string_from_bytes(schema->name, block_string_name->content);

    schema->attr_count = db_schema->attr_count;

    schema->attributes = malloc(schema->attr_count * sizeof(*schema->attributes));
    for (amount i = 0; i < schema->attr_count; i++) {
        connection_move_to_pointer(db_schema->attr_pointers[i], connection);
        Block *block_attribute = block_init();
        block_from_file(block_attribute, connection);
        DB_Attribute *db_attribute = db_attribute_init();
        db_attribute_from_bytes(db_attribute, block_attribute->content);
        schema->attributes[i] = attribute_init();
        db_attribute_to_attribute(db_attribute, schema->attributes[i], connection);
    }
    return RC_OK;
}

RC db_schema_from_schema(DB_Schema *db_schema, Schema *schema) {
    db_schema->name_string_pointer = nullptr;
    db_schema->attr_count = schema->attr_count;
    db_schema->schema_next_pointer = nullptr;
    db_schema->attr_pointers = malloc(db_schema->attr_count * sizeof(*db_schema->attr_pointers));
    return RC_OK;
}
