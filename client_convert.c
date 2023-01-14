#include <stdio.h>
#include "client_convert.h"


#define ARR_CONVERTER(Item, item) \
GPtrArray *convert_##item##_list(Item *item) { \
    GPtrArray *array = g_ptr_array_new(); \
    while (item) { \
        g_ptr_array_add(array, convert_##item(item)); \
        item = item->next; \
    } \
    return array; \
} \


I_Value *convert_value(Value *value) {
    I_uValue *u_value = g_object_new(TYPE_I_U_VALUE, NULL);
    I_Value *result = g_object_new(TYPE_I__VALUE,
                                   "type", value->vt,
                                   "value", u_value,
                                   NULL);

    switch (value->vt) {
        case VT_INTEGER:
            g_object_set(u_value,
                         "integer", value->integer,
                         NULL);
            break;
        case VT_STRING:
            g_object_set(u_value,
                         "string_", g_strdup(value->string),
                         NULL);
            break;
        case VT_FLOATING:
            g_object_set(u_value,
                         "floating", value->floating,
                         NULL);
            break;
        case VT_BOOLEAN:
            g_object_set(u_value,
                         "boolean", value->boolean,
                         NULL);
            break;
    }

    return result;
}

I_Attribute *convert_attribute(Attribute *attribute) {
    I_Attribute *result = g_object_new(TYPE_I__ATTRIBUTE,
                                       "name", g_strdup(attribute->name),
                                       "vt", attribute->vt,
                                       NULL);
    return result;
}

ARR_CONVERTER(Attribute, attribute)

I_Property *convert_property(Property *property) {
    I_Property *result = g_object_new(TYPE_I__PROPERTY,
                                      "field", g_strdup(property->field),
                                      "value", convert_value(property->value),
                                      NULL);
    return result;
}

GPtrArray *convert_property_list(Property *property) {
    GPtrArray *array = g_ptr_array_new();
    while (property) {
        g_ptr_array_insert(array, 0, convert_property(property));
        property = property->next;
    }
    return array;
}

I_Schema *convert_schema(Schema *schema) {
    I_Schema *result = g_object_new(TYPE_I__SCHEMA,
                                    "name", g_strdup(schema->name),
                                    "attributes", convert_attribute_list(schema->attributes),
                                    NULL);
    return result;
}

I_Node *convert_node(Node *node) {
    I_Node *result = g_object_new(TYPE_I__NODE,
                                  "name", g_strdup(node->name),
                                  "properties", convert_property_list(node->properties),
                                  NULL);
    return result;
}

I_SchemaCreateQuery *convert_schema_create_query(SchemaCreateQuery query) {
    I_SchemaCreateQuery *result = g_object_new(
            TYPE_I__SCHEMA_CREATE_QUERY,
            "schema", convert_schema(query.schema),
            NULL);
    return result;
}

I_SchemaGetQuery *convert_schema_get_query(SchemaGetQuery query) {
    I_SchemaGetQuery *result = g_object_new(
            TYPE_I__SCHEMA_GET_QUERY,
            "schema_name", query.schema_name,
            NULL);
    return result;
}

I_SchemaDeleteQuery *convert_schema_delete_query(SchemaDeleteQuery query) {
    I_SchemaDeleteQuery *result = g_object_new(
            TYPE_I__SCHEMA_DELETE_QUERY,
            "schema_name", query.schema_name,
            NULL);
    return result;
}


I_NodeCreateQuery *convert_node_create_query(NodeCreateQuery query) {
    I_NodeCreateQuery *result = g_object_new(
            TYPE_I__NODE_CREATE_QUERY,
            "schema_name", query.schema_name,
            "node", convert_node(query.node),
            NULL);
    return result;
}

I_Comparable *convert_comparable(Comparable *comparable) {
    I_uComparable *u_comparable = g_object_new(TYPE_I_U_COMPARABLE, NULL);
    I_Comparable *result = g_object_new(TYPE_I__COMPARABLE,
                                        "type", comparable->type,
                                        "comparable", u_comparable,
                                        NULL);
    switch (comparable->type) {
        case CMPT_FIELD:
            g_object_set(u_comparable,
                         "field_name", g_strdup(comparable->field_name),
                         NULL);
            break;
        case CMPT_VALUE:
            g_object_set(u_comparable,
                         "value", convert_value(comparable->value),
                         NULL);
            break;
    }
    return result;
}

I_Compare *convert_compare(Compare *compare) {
    I_Compare *result = g_object_new(TYPE_I__COMPARE,
                                     "cmp", compare->cmp,
                                     "with_", convert_comparable(compare->with),
                                     NULL);
    return result;
}

I_ConditionCompare *convert_condition_compare(ConditionCompare *condition_compare) {
    I_ConditionCompare *result = g_object_new(TYPE_I__CONDITION_COMPARE,
                                              "left", convert_comparable(condition_compare->left),
                                              "compare", convert_compare(condition_compare->compare),
                                              NULL);
    return result;
}

I_Predicate *convert_predicate(Predicate *predicate);

GPtrArray *convert_predicate_list(Predicate *predicate) {
    GPtrArray *array = g_ptr_array_new();
    while (predicate) {
        g_ptr_array_add(array, convert_predicate(predicate));
        predicate = predicate->next;
    }
    return array;
}

I_Predicate *convert_predicate(Predicate *predicate) {
    I_uPredicate *u_predicate = g_object_new(TYPE_I_U_PREDICATE, NULL);
    I_Predicate *result = g_object_new(TYPE_I__PREDICATE,
                                       "type", predicate->type,
                                       "predicate", u_predicate,
                                       NULL);
    switch (predicate->type) {
        case PT_TERM:
            g_object_set(u_predicate,
                         "term", convert_condition_compare(predicate->term),
                         NULL);
            break;
        case PT_OR:
            g_object_set(u_predicate,
                         "or_", convert_predicate_list(predicate->or),
                         NULL);
            break;
        case PT_AND:
            g_object_set(u_predicate,
                         "and_", convert_predicate_list(predicate->and),
                         NULL);
            break;
    }
    return result;
}

GPtrArray *convert_predicates(Predicate *predicates) {
    GPtrArray *array = g_ptr_array_new();
    while (predicates) {
        g_ptr_array_add(array, convert_predicate(predicates));
        predicates = predicates->next;
    }
    return array;
}

I_NodeCondition *convert_node_condition(NodeCondition *node_condition) {
    I_NodeCondition *result = g_object_new(
            TYPE_I__NODE_CONDITION,
            NULL);
    if (!node_condition)
        g_object_set(result,
                     "is_null", true,
                     NULL);
    else {
        g_object_set(result,
                "is_null", node_condition->is_null,
                "schema", g_strdup(node_condition->schema),
                "predicates", convert_predicates(node_condition->predicates),
                NULL);
    }
    return result;
}

I_NodeMatchQuery *convert_node_match_query(NodeMatchQuery query) {
    I_NodeMatchQuery *result = g_object_new(
            TYPE_I__NODE_MATCH_QUERY,
            "condition", convert_node_condition(query.condition),
            NULL);
    return result;
}

I_NodeUpdateQuery *convert_node_update_query(NodeUpdateQuery query) {
    I_NodeUpdateQuery *result = g_object_new(
            TYPE_I__NODE_UPDATE_QUERY,
            "condition", convert_node_condition(query.condition),
            "properties", convert_property_list(query.properties),
            NULL);
    return result;
}

I_NodeDeleteQuery *convert_node_delete_query(NodeDeleteQuery query) {
    I_NodeDeleteQuery *result = g_object_new(
            TYPE_I__NODE_DELETE_QUERY,
            "condition", convert_node_condition(query.condition),
            NULL);
    return result;
}

I_LinkCondition *convert_link_condition(LinkCondition *condition) {
    I_LinkCondition *result = g_object_new(
            TYPE_I__LINK_CONDITION,
            "is_null", condition->is_null,
            "link_type", condition->link_type,
            "link_name", g_strdup(condition->link_name),
            NULL);
    return result;
}

I_MatchCondition *convert_match_condition(MatchCondition *condition) {
    I_MatchCondition *result = g_object_new(
            TYPE_I__MATCH_CONDITION,
            "is_null", condition->is_null,
            "node_first_condition", convert_node_condition(condition->node_first_condition),
            "node_second_condition", convert_node_condition(condition->node_second_condition),
            "node_cross_condition", convert_node_condition(condition->node_cross_condition),
            "link_condition", convert_link_condition(condition->link_condition),
            NULL);
    return result;
}

I_LinkCreateQuery *convert_link_create_query(LinkCreateQuery query) {
    gchar *link_name = g_strdup(query.name);
    I_LinkType link_type = (I_LinkType) query.link_type;
    I_NodeCondition *first = convert_node_condition(query.first);
    I_NodeCondition *second = convert_node_condition(query.second);

    I_LinkCreateQuery *result = g_object_new(
            TYPE_I__LINK_CREATE_QUERY,
            NULL);
    g_object_set(result,
                 "link_name", link_name,
                 "link_type", link_type,
                 "first", first,
                 "second", second,
                 NULL);
    return result;
}

I_LinkDeleteQuery *convert_link_delete_query(LinkDeleteQuery query) {
    I_LinkDeleteQuery *result = g_object_new(
            TYPE_I__LINK_DELETE_QUERY,
            "condition", convert_match_condition(query.condition),
            NULL);
    return result;
}

I_MatchQuery *convert_match_query(MatchQuery query) {
    I_MatchQuery *result = g_object_new(
            TYPE_I__MATCH_QUERY,
            "condition", convert_match_condition(query.condition),
            NULL);
    return result;
}

I_Query *convert_query(Query *query) {
    I_Query *result = g_object_new(TYPE_I__QUERY, NULL);
    I_uQuery *u_query = g_object_new(TYPE_I_U_QUERY, NULL);
    g_object_set(
            result,
            "type", query->type,
            "query", u_query,
            NULL);

    switch (query->type) {
        case SCHEMA_CREATE:
            g_object_set(u_query,
                         "schema_create_query",
                         convert_schema_create_query(query->schema_create_query),
                         NULL);
            break;
        case SCHEMA_GET:
            g_object_set(u_query,
                         "schema_get_query",
                         convert_schema_get_query(query->schema_get_query),
                         NULL);
            break;
        case SCHEMA_GET_ALL:
            g_object_set(u_query,
                         "schema_get_all_query",
                         g_object_new(TYPE_I__SCHEMA_GET_ALL_QUERY, NULL),
                         NULL);
            break;
        case SCHEMA_DELETE:
            g_object_set(u_query,
                         "schema_delete_query",
                         convert_schema_delete_query(query->schema_delete_query),
                         NULL);
            break;
        case NODE_CREATE:
            g_object_set(u_query,
                         "node_create_query",
                         convert_node_create_query(query->node_create_query),
                         NULL);
            break;
        case NODE_UPDATE:
            g_object_set(u_query,
                         "node_update_query",
                         convert_node_update_query(query->node_update_query),
                         NULL);
            break;
        case NODE_DELETE:
            g_object_set(u_query,
                         "node_delete_query",
                         convert_node_delete_query(query->node_delete_query),
                         NULL);
            break;
        case NODE_MATCH:
            g_object_set(u_query,
                         "node_match_query",
                         convert_node_match_query(query->node_match_query),
                         NULL);
            break;
        case LINK_CREATE:
            g_object_set(u_query,
                         "link_create_query",
                         convert_link_create_query(query->link_create_query),
                         NULL);
            break;
        case LINK_DELETE:
            g_object_set(u_query,
                         "link_delete_query",
                         convert_link_delete_query(query->link_delete_query),
                         NULL);
            break;
        case MATCH:
            g_object_set(u_query,
                         "match_query",
                         convert_match_query(query->match_query),
                         NULL);
            break;
    }
    return result;
}

void print_attribute(I_Attribute *attribute) {
    gchar *name;
    I_ValueType type;
    g_object_get(attribute,
                 "name", &name,
                 "vt", &type,
                 NULL);
    printf("%s: %s", name, toString_I_ValueType(type));
}

void print_schema(I_Schema *schema) {
    gchar *name;
    GPtrArray *attributes;
    g_object_get(schema,
                 "name", &name,
                 "attributes", &attributes,
                 NULL);
    printf("Schema: %s", name);
    for (size_t i = 0; i < attributes->len; i++) {
        printf("\n");
        printf("- ");
        print_attribute(attributes->pdata[i]);
    }
}

const int padlen = 10;

void print_value(I_Value *value) {
    I_ValueType type;
    I_uValue *u_value;
    g_object_get(value,
                 "type", &type,
                 "value", &u_value,
                 NULL);
    switch (type) {
        case I__VALUE_TYPE_VT_INTEGER:
            printf("%*d", padlen, u_value->integer);
            break;
        case I__VALUE_TYPE_VT_STRING:
            printf("%*s", padlen, u_value->string_);
            break;
        case I__VALUE_TYPE_VT_FLOATING:
            printf("%*f", padlen, u_value->floating);
            break;
        case I__VALUE_TYPE_VT_BOOLEAN:
            printf("%*s", padlen, u_value->boolean ? "true" : "false");
            break;
    }
}

void print_property(I_Property *property) {
//    printf("%s: ",property->field);
    print_value(property->value);
}

void print_node(I_Node *node) {
    gchar *name;
    GPtrArray *properties;
    g_object_get(node,
                 "name", &name,
                 "properties", &properties,
                 NULL);

    if (name) printf("%s", name);
    printf("[");
    for (size_t i = 0; i < properties->len; i++) {
        print_property(properties->pdata[i]);
//        printf(", ");
    }
    printf("]");
}

void print_link(I_Link *link) {
    I_LinkType type;
    gchar *name;
    g_object_get(link,
                 "type", &type,
                 "name", &name,
                 NULL);
    if (type == LT_FROM || type == LT_BOTH)
        printf("<");
    printf("--[%s]--", name);
    if (type == LT_TO || type == LT_BOTH)
        printf(">");
}

void print_result_item(I_Result *i_result) {
    I_Node *first, *second;
    I_Link *link;
    I_Schema *schema;
    g_object_get(i_result,
                 "first", &first,
                 "second", &second,
                 "link", &link,
                 "schema", &schema,
                 NULL);

    if (i_result->__isset_schema) {
        print_schema(schema);
    } else if (
            i_result->__isset_first ||
            i_result->__isset_link ||
            i_result->__isset_second
    ) {
        if (i_result->__isset_first)
            print_node(first);
        if (i_result->__isset_link)
            print_link(link);
        if (i_result->__isset_second)
            print_node(second);
    }
}

void print_items(GPtrArray *items) {
    for (size_t i = 0; i < items->len; i++) {
        printf("%zu. ", i);
        print_result_item(items->pdata[i]);
        printf("\n");
    }
}

void print_result(I_QueryResult *result) {
    I_QueryResultType type;
    gchar *message;
    I_uQueryResult *u_query;
    g_object_get(result,
                 "type", &type,
                 "message", &message,
                 "result", &u_query,
                 NULL);

    printf("%s\n", message);

    I_Schema *i_schema;
    GPtrArray *items;
    switch (type) {
        case I__QUERY_RESULT_TYPE_RESULT_SCHEMA:
            g_object_get(u_query,
                         "schema", &i_schema,
                         NULL);
            print_schema(i_schema);
            printf("\n");
            break;
        case I__QUERY_RESULT_TYPE_RESULT_GENERATOR:
            g_object_get(u_query,
                         "items", &items,
                         NULL);
            print_items(items);
            break;
        case I__QUERY_RESULT_TYPE_RESULT_NONE:
            break;
    }
}

