#include <string.h>
#include <malloc.h>
#include "db_model.h"

Value *create_value(ValueType vt) {
    Value *v = malloc(sizeof(Value));
    v->vt = vt;
    return v;
}

Value *create_integer(int integer) {
    Value *v = create_value(VT_INTEGER);
    v->integer = integer;
    return v;
}

Value *create_string(char *string) {
    Value *v = create_value(VT_STRING);
    v->string = strdup(string);
    return v;
}

Value *create_floating(float floating) {
    Value *v = create_value(VT_FLOATING);
    v->floating = floating;
    return v;
}

Value *create_boolean(bool boolean) {
    Value *v = create_value(VT_BOOLEAN);
    v->boolean = boolean;
    return v;
}

void free_value(Value *value) {
    if (value->vt == VT_STRING)
        free(value->string);
    free(value);
}

Node *create_node() {
    Node *x = malloc(sizeof(Node));
    x->name = NULL;
    return x;
}

void free_node(Node *node) {
    if (node->name) free(node->name);
    if (node->properties) free_property(node->properties);
    free(node);
}

Link *create_link(LinkType type, char *first, char *second) {
    Link *x = malloc(sizeof(Link));
    x->type = type;
    x->first = strdup(first);
    x->second = strdup(second);
    return x;
}

void free_link(Link *link) {
    if (link->first) free(link->first);
    if (link->second) free(link->second);
    free(link);
}

Property *create_property() {
    return malloc(sizeof(Property));
}

void free_property(Property *property) {
    free_value(property->value);
    if (property->field) free(property->field);
    if (property->next) free_property(property->next);
    free(property);
}

Attribute *create_attribute() {
    Attribute *attribute = malloc(sizeof(Attribute));
    return attribute;
}

void free_attribute(Attribute *attribute) {
    if (attribute->name) free(attribute->name);
    if (attribute->next) free_attribute(attribute->next);
    free(attribute);
}

Schema *create_schema() {
    Schema *schema = malloc(sizeof(Schema));
    return schema;
}

void free_schema(Schema *schema) {
    if (schema->name) free(schema->name);
    if (schema->attributes) free(schema->attributes);
    free(schema);
}
