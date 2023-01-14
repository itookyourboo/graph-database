#include "value_crud.h"
#include "string_crud.h"

DEFINE_INSERT_FUNCTION(DB_Value, db_value)
DEFINE_RETRIEVE_FUNCTION(DB_Value, db_value)
DEFINE_UPDATE_FUNCTION(DB_Value, db_value)

RC value_insert(Value *value, pointer *insertion_pointer, Connection *connection) {
    RC rc = RC_OK;

    DB_Value *db_value = &(DB_Value) {
            .data_type = value->data_type
    };

    if (value->data_type == DT_STRING) {
        pointer block_string_pointer;
        rc = string_insert(value->string, &block_string_pointer, connection);
        throw_if_not_ok(rc);
        db_value->string_pointer = block_string_pointer;
    } else {
        db_value->integer = value->integer;
    }

    rc = db_value_insert(db_value, insertion_pointer, connection);
    throw_if_not_ok(rc);
    return rc;
}

RC value_retrieve(Value *value, Connection *connection) {
    RC rc = RC_OK;
    DB_Value *db_value = db_value_init();
    rc = db_value_retrieve(db_value, connection);
    throw_if_not_ok(rc);

    value->data_type = db_value->data_type;

    if (db_value->data_type == DT_STRING) {
        MV_SAVE(db_value->string_pointer);
        value->string = string_init();
        rc = string_retrieve(value->string, connection);
        MV_BACK();
    } else {
        value->integer = db_value->integer;
    }

    db_value_free(db_value);
    return rc;
}

RC value_update(Value *new_value, Connection *connection) {
    RC rc = RC_OK;
    SAVE();
    DB_Value *db_value = db_value_init();
    rc = db_value_retrieve(db_value, connection);
    throw_if_not_ok(rc);

    if (
            db_value->data_type == DT_STRING && new_value->data_type != DT_STRING ||
            db_value->data_type != DT_STRING && new_value->data_type == DT_STRING
    )
        return RC_DATA_TYPES_ARE_NOT_EQUAL;

    if (db_value->data_type == DT_STRING) {
        database_free_blocks((pointer[1]){db_value->string_pointer}, 1, connection);

        pointer block_string_pointer;
        rc = string_insert(new_value->string, &block_string_pointer, connection);
        throw_if_not_ok(rc);
        db_value->string_pointer = block_string_pointer;
    } else {
        if (db_value->data_type == DT_FLOATING && new_value->data_type == DT_FLOATING)
            db_value->floating = new_value->floating;
        else if (db_value->data_type == DT_FLOATING)
            db_value->floating = (float) new_value->integer;
        else if (new_value->floating == DT_FLOATING)
            db_value->integer = (int) new_value->floating;
        else
            db_value->integer = new_value->integer;
    }

    BACK();

    db_value_update(db_value, connection);
    db_value_free(db_value);

    return rc;
}
