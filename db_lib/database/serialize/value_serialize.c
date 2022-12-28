#include "value_serialize.h"


#include "util.h"
#include "block_serialize.h"
#include "string_serialize.h"


RC db_value_to_bytes(DB_Value *db_value, byte *bytes) {
    RC rc = RC_OK;
    amount i = 0;

    field_to_bytes(db_value, data_type, bytes, i);
    switch (db_value->data_type) {
        case DT_INTEGER:
            field_to_bytes(db_value, integer, bytes, i);
            break;
        case DT_FLOATING:
            field_to_bytes(db_value, floating, bytes, i);
            break;
        case DT_BOOLEAN:
            field_to_bytes(db_value, boolean, bytes, i);
            break;
        case DT_STRING:
            field_to_bytes(db_value, string_pointer, bytes, i);
            break;
    }
    return rc;
}

RC db_value_from_bytes(DB_Value *db_value, byte *bytes) {
    RC rc = RC_OK;
    amount i = 0;

    field_from_bytes(db_value, data_type, bytes, i);
    switch (db_value->data_type) {
        case DT_INTEGER:
            field_from_bytes(db_value, integer, bytes, i);
            break;
        case DT_FLOATING:
            field_from_bytes(db_value, floating, bytes, i);
            break;
        case DT_BOOLEAN:
            field_from_bytes(db_value, boolean, bytes, i);
            break;
        case DT_STRING:
            field_from_bytes(db_value, string_pointer, bytes, i);
            break;
    }
    return rc;
}


RC db_value_to_value(DB_Value *db_value, Value *value, Connection *connection) {
    RC rc = RC_OK;
    value->data_type = db_value->data_type;

    if (db_value->data_type == DT_STRING) {
        MV_SAVE(db_value->string_pointer);
        Block *block_string_name = block_init();
        rc = block_from_file(block_string_name, connection);
        throw_if_not_ok(rc);
        value->string = string_init();
        rc = string_from_bytes(value->string, block_string_name->content);
        throw_if_not_ok(rc);
        MV_BACK();
    } else {
        value->integer = db_value->integer;
    }
    return rc;
}