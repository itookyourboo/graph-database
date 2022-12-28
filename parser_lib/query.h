#ifndef LOW_LEVEL_PROGRAMMING_TASK2_QUERY_H
#define LOW_LEVEL_PROGRAMMING_TASK2_QUERY_H

#include "db_model.h"

#define NO_CONDITION {.is_null = true}

typedef enum {
    CMP_GT,
    CMP_LT,
    CMP_GE,
    CMP_LE,
    CMP_EQ,
    CMP_NEQ,
    CMP_CONTAINS
} Comparation, CMP;

typedef enum {
    CMPT_FIELD,
    CMPT_VALUE,
} ComparableType;

typedef struct {
    ComparableType type;
    union {
        char *field_name;
        Value *value;
    };
} Comparable;

Comparable *create_comparable(ComparableType type);
void free_comparable(Comparable *comparable);

typedef struct {
    CMP cmp;
    Comparable *with;
} Compare;

Compare *create_compare(CMP cmp, Comparable *with);
void free_compare(Compare *compare);

typedef struct ConditionCompare {
    Comparable *left;
    Compare *compare;
} ConditionCompare;

ConditionCompare *create_condition(Comparable *left, Compare *compare);
void free_condition_compare(ConditionCompare *condition_compare);

typedef enum {
    PT_TERM,
    PT_OR,
    PT_AND
} PredicateType;

typedef struct Predicate {
    PredicateType type;
    union {
        ConditionCompare *term;
        struct Predicate *and;
        struct Predicate *or;
    };
    struct Predicate *next;
} Predicate;

Predicate *create_predicate(PredicateType type);
void free_predicate(Predicate *predicate);

typedef struct {
    bool is_null;
    char *schema;
    Predicate *predicates;
} NodeCondition;

NodeCondition *create_node_condition();
void free_node_condition(NodeCondition *node_condition);

typedef struct {
    bool is_null;

    LinkType link_type;
    char *link_name;
} LinkCondition;

LinkCondition *create_link_condition();
void free_link_condition(LinkCondition *link_condition);

typedef struct {
    bool is_null;

    NodeCondition *node_first_condition;
    NodeCondition *node_second_condition;
    NodeCondition *node_cross_condition;
    LinkCondition *link_condition;
} MatchCondition;

MatchCondition *create_match_condition();
void free_match_condition(MatchCondition *match_condition);

typedef enum {
    SCHEMA_CREATE,
    SCHEMA_GET,
    SCHEMA_GET_ALL,
    SCHEMA_DELETE,

    NODE_CREATE,
    NODE_UPDATE,
    NODE_DELETE,
    NODE_MATCH,

    LINK_CREATE,
    LINK_DELETE,

    MATCH
} QueryType;

typedef struct {
    Schema *schema;
} SchemaCreateQuery;

typedef struct {
    char *schema_name;
} SchemaGetQuery;

typedef struct {
    char *schema_name;
} SchemaDeleteQuery;

typedef struct {
    char *schema_name;
    Node *node;
} NodeCreateQuery;

typedef struct {
    NodeCondition *condition;
    Property *properties;
} NodeUpdateQuery;

typedef struct {
    NodeCondition *condition;
} NodeDeleteQuery;

typedef struct {
    NodeCondition *condition;
} NodeMatchQuery;

typedef struct {
    Link *link;
} LinkCreateQuery;

typedef struct {
    MatchCondition *condition;
} LinkDeleteQuery;

typedef struct {
    MatchCondition *condition;
} MatchQuery;

typedef enum {
    RESULT_SCHEMA,
    RESULT_GENERATOR,
    RESULT_NONE
} QueryResultType;

typedef struct {
    QueryType type;
    union {
        SchemaCreateQuery schema_create_query;
        SchemaGetQuery schema_get_query;
        SchemaDeleteQuery schema_delete_query;

        NodeCreateQuery node_create_query;
        NodeUpdateQuery node_update_query;
        NodeDeleteQuery node_delete_query;
        NodeMatchQuery node_match_query;

        LinkCreateQuery link_create_query;
        LinkDeleteQuery link_delete_query;

        MatchQuery match_query;
    };
} Query;

Query *create_query(QueryType type);
void free_query(Query *query);

#endif //LOW_LEVEL_PROGRAMMING_TASK2_QUERY_H