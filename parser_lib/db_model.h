#ifndef LOW_LEVEL_PROGRAMMING_TASK2_DB_MODEL_H
#define LOW_LEVEL_PROGRAMMING_TASK2_DB_MODEL_H

#include "stdbool.h"

typedef enum {
    VT_INTEGER,
    VT_STRING,
    VT_FLOATING,
    VT_BOOLEAN
} ValueType;

typedef struct {
    ValueType vt;
    union {
        int integer;
        char *string;
        float floating;
        bool boolean;
    };
} Value;

Value *create_integer(int integer);
Value *create_string(char *string);
Value *create_floating(float floating);
Value *create_boolean(bool boolean);

void free_value(Value *value);

typedef struct Property {
    char *field;
    Value *value;
    struct Property *next;
} Property;

Property *create_property();
void free_property(Property *property);

typedef struct Attribute {
    char *name;
    ValueType vt;
    struct Attribute *next;
} Attribute;

Attribute *create_attribute();
void free_attribute(Attribute *attribute);

typedef struct {
    char *name;
    Attribute *attributes;
} Schema;

Schema *create_schema();
void free_schema(Schema *schema);

typedef struct {
    char *name;
    Property *properties;
} Node;

Node *create_node();
void free_node(Node *node);

typedef enum {
    LT_TO,
    LT_FROM,
    LT_BOTH,

    LT_ANY,
} LinkType;

typedef struct {
    char *name;
    LinkType type;
    char *first;
    char *second;
} Link;

Link *create_link(LinkType type, char *first, char *second);
void free_link(Link *link);


#endif //LOW_LEVEL_PROGRAMMING_TASK2_DB_MODEL_H
