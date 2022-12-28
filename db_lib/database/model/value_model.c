#include <malloc.h>
#include <string.h>
#include "value_model.h"


DB_Value *db_value_init() {
    DB_Value *db_value = malloc(sizeof(DB_Value));
    return db_value;
}

amount db_value_get_data_type_size(DB_Value *db_value) {
    switch (db_value->data_type) {
        case DT_INTEGER:
            return sizeof(db_value->integer);
        case DT_STRING:
            return sizeof(db_value->string_pointer);
        case DT_FLOATING:
            return sizeof(db_value->floating);
        case DT_BOOLEAN:
            return sizeof(db_value->boolean);
    }
}

amount db_value_size(DB_Value *value) {
    return (
            sizeof(value->data_type) +
            db_value_get_data_type_size(value)
    );
}

void db_value_free(DB_Value *value) {
    free(value);
}


Value *value_init() {
    Value *value = malloc(sizeof(Value));
    return value;
}

amount value_get_data_type_size(Value *value) {
    switch (value->data_type) {
        case DT_INTEGER:
            return sizeof(value->integer);
        case DT_STRING:
            return string_size(value->string);
        case DT_FLOATING:
            return sizeof(value->floating);
        case DT_BOOLEAN:
            return sizeof(value->boolean);
    }
}

amount value_size(Value *value) {
    return (
            sizeof(value->data_type) +
            value_get_data_type_size(value)
    );
}

void value_free(Value *value) {
    if (value->data_type == DT_STRING)
        string_free(value->string);
    free(value);
}

#define SIGN(x, y) (((x) > (y)) - ((x) < (y)))

CMP value_compare(Value *value1, Value *value2) {
    if (value1 == NULL || value2 == NULL)
        return CMP_UNDEFINED;
    if (value1->data_type != value2->data_type)
        return CMP_UNDEFINED;

    int res;
    if (value1->data_type == DT_STRING)
        res = strncmp(value1->string->content, value2->string->content, value1->string->size);
    else if (value1->data_type == DT_FLOATING)
        res = SIGN(value1->floating, value2->floating);
    else if (value1->data_type == DT_INTEGER)
        res = SIGN(value1->integer, value2->integer);
    else if (value1->data_type == DT_BOOLEAN)
        res = value1->boolean - value2->boolean;

    if (res < 0) return CMP_LESS;
    else if (res == 0) return CMP_EQUAL;
    else if (res > 0) return CMP_GREATER;
}

Value *value_copy(Value *src) {
    Value *value = value_init();
    value->data_type = src->data_type;
    if (value->data_type == DT_STRING)
        value->string = string_copy(src->string);
    else
        value->integer = src->integer;
    return value;
}