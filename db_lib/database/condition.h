#ifndef LOW_LEVEL_PROGRAMMING_TASK1_CONDITION_H
#define LOW_LEVEL_PROGRAMMING_TASK1_CONDITION_H

#include "model/models.h"

typedef enum {
    CMPT_FIELD_NAME,
    CMPT_VALUE,
} ComparableType;

typedef struct {
    ComparableType type;
    union {
        char *field_name;
        Value *value;
    };
} Comparable;

typedef struct {
    Comparable left;
    CMP cmp;
    Comparable right;
} ConditionCompare;

typedef enum {
    CT_LINK,
    CT_CONNECTED,
    CT_COMPARE
} ConditionType;

typedef struct {
    bool is_null;

    Schema *schema;

    amount conditions_num;
    ConditionCompare *compare_conditions;
} NodeCondition;

typedef struct {
    bool is_null;

    LinkType link_type;
    String *link_name;
} LinkCondition;

typedef struct {
    bool is_null;

    char *field_name_first;
    CMP cmp;
    char *field_name_second;
} CrossNodeCondition;

typedef struct {
    bool is_null;

    NodeCondition node_first_condition;
    NodeCondition node_second_condition;
    NodeCondition node_cross_condition;
    LinkCondition link_condition;
} MatchCondition;


#define NO_CONDITION {.is_null = true}


#endif //LOW_LEVEL_PROGRAMMING_TASK1_CONDITION_H
