#include <malloc.h>
#include <string.h>
#include "attribute_model.h"


DB_Attribute *db_attribute_init() {
    DB_Attribute *db_attribute = malloc(sizeof(DB_Attribute));
    return db_attribute;
}

amount db_attribute_size(DB_Attribute *db_attribute) {
    return sizeof(DB_Attribute);
}

void db_attribute_free(DB_Attribute *db_attribute) {
    free(db_attribute);
}

Attribute *attribute_init() {
    Attribute *attribute = malloc(sizeof(Attribute));
    return attribute;
}

amount attribute_size(Attribute *attribute) {
    return (
            string_size(attribute->name) +
            sizeof(attribute->data_type)
    );
}

void attribute_free(Attribute *attribute) {
    string_free(attribute->name);
    free(attribute);
}

Attribute *attribute_copy(Attribute *src) {
    Attribute *attribute = attribute_init();
    memcpy(attribute, src, sizeof(Attribute));
    attribute->name = string_copy(src->name);
    return attribute;
}