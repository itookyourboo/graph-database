#include "query.h"
#include <malloc.h>


Query *create_query(QueryType type) {
    Query *q = malloc(sizeof(Query));
    q->type = type;
    return q;
}

void free_query(Query *query) {
    switch (query->type) {
        case SCHEMA_CREATE:
            free_schema(query->schema_create_query.schema);
            break;
        case SCHEMA_GET:
            free(query->schema_get_query.schema_name);
            break;
        case SCHEMA_GET_ALL:
            break;
        case SCHEMA_DELETE:
            free(query->schema_delete_query.schema_name);
            break;
        case NODE_CREATE:
            free(query->node_create_query.schema_name);
            free_node(query->node_create_query.node);
            break;
        case NODE_UPDATE:
            free_property(query->node_update_query.properties);
            free_node_condition(query->node_update_query.condition);
            break;
        case NODE_DELETE:
            free_node_condition(query->node_delete_query.condition);
            break;
        case NODE_MATCH:
            free_node_condition(query->node_match_query.condition);
            break;
        case LINK_CREATE:
//            free_link(query->link_create_query.link);
            break;
        case LINK_DELETE:
            free_match_condition(query->link_delete_query.condition);
            break;
        case MATCH:
            free_match_condition(query->match_query.condition);
            break;
    }
    free(query);
}

Comparable *create_comparable(ComparableType type) {
    Comparable *x = malloc(sizeof(Comparable));
    x->type = type;
    return x;
}

void free_comparable(Comparable *comparable) {
    if (comparable->field_name) free(comparable->field_name);
    if (comparable->value) free_value(comparable->value);
    free(comparable);
}

Compare *create_compare(CMP cmp, Comparable *with) {
    Compare *x = malloc(sizeof(Compare));
    x->cmp = cmp;
    x->with = with;
    return x;
}

void free_compare(Compare *compare) {
    if (compare->with) free_comparable(compare->with);
    free(compare);
}

ConditionCompare *create_condition(Comparable *left, Compare *compare) {
    ConditionCompare *x = malloc(sizeof(ConditionCompare));
    x->left = left;
    x->compare = compare;
    return x;
}

void free_condition_compare(ConditionCompare *condition_compare) {
    if (condition_compare->compare) free_compare(condition_compare->compare);
    if (condition_compare->left) free_comparable(condition_compare->left);
    free(condition_compare);
}

Predicate *create_predicate(PredicateType type) {
    Predicate *x = malloc(sizeof(Predicate));
    x->type = type;
    return x;
}

void free_predicate(Predicate *predicate) {
    switch (predicate->type) {
        case PT_TERM:
            free_condition_compare(predicate->term);
            break;
        case PT_OR:
            free_predicate(predicate->or);
            break;
        case PT_AND:
            free_predicate(predicate->and);
            break;
    }
    if (predicate->next) free_predicate(predicate->next);
    free(predicate);
}

NodeCondition *create_node_condition() {
    NodeCondition *x = malloc(sizeof(NodeCondition));
    x->predicates = NULL;
    return x;
}

void free_node_condition(NodeCondition *node_condition) {
    if (node_condition->schema) free(node_condition->schema);
    if (node_condition->predicates) free(node_condition->predicates);
    free(node_condition);
}

MatchCondition *create_match_condition() {
    MatchCondition *x = malloc(sizeof(MatchCondition));
    x->node_cross_condition = create_node_condition();
    x->node_cross_condition->is_null = true;
    return x;
}

LinkCondition *create_link_condition() {
    LinkCondition *x = malloc(sizeof(LinkCondition));
    return x;
}

void free_link_condition(LinkCondition *link_condition) {
    if (link_condition->link_name) free(link_condition->link_name);
    free(link_condition);
}

void free_match_condition(MatchCondition *match_condition) {
    if (match_condition->link_condition) free_link_condition(match_condition->link_condition);
    if (match_condition->node_first_condition) free_node_condition(match_condition->node_first_condition);
    if (match_condition->node_second_condition) free_node_condition(match_condition->node_second_condition);
    if (match_condition->node_cross_condition) free_node_condition(match_condition->node_cross_condition);
    free(match_condition);
}
