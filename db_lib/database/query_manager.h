#ifndef LOW_LEVEL_PROGRAMMING_TASK1_QUERY_MANAGER_H
#define LOW_LEVEL_PROGRAMMING_TASK1_QUERY_MANAGER_H

#include "model/schema_model.h"
#include "model/node_model.h"
#include "condition.h"
#include "connection.h"
#include "data_structure/generator.h"
#include "data_structure/vector.h"

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
    Schema *schema;
    Node *node;
} NodeCreateQuery;

typedef struct {
    NodeCondition condition;
} NodeMatchQuery;

typedef struct {
    NodeCondition condition;
    amount attributes_to_update_number;
    char **attributes_to_update;
    Value **new_values;
} NodeUpdateQuery;

typedef struct {
    NodeCondition condition;
} NodeDeleteQuery;

typedef struct {
    Link *link;
    NodeCondition first;
    NodeCondition second;
//    Node *first;
//    Node *second;
} LinkCreateQuery;

typedef struct {
    LinkVector *link_vector;
    String new_name;
    LinkType new_type;
} LinkUpdateQuery;

typedef struct {
    MatchCondition condition;
} LinkDeleteQuery;

typedef struct {
    MatchCondition condition;
    Return return_items;
} MatchQuery;

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
        LinkUpdateQuery link_update_query;
        LinkDeleteQuery link_delete_query;

        MatchQuery match_query;
    };
} Query;

typedef enum {
    RESULT_SCHEMA,
    RESULT_GENERATOR,
    RESULT_NONE
} QueryResultType;

typedef struct {
    QueryResultType type;
    union {
        Schema *schema;
        Generator *generator;
    };
    RC status;
} QueryResult;

QueryResult query_execute(Query query, Connection *connection);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_QUERY_MANAGER_H
