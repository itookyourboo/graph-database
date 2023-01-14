#include <stdio.h>
#include "printer.h"

const char *value_types[] = {
        "INTEGER",
        "STRING",
        "FLOATING",
        "BOOLEAN"
};

const char *query_types[] = {
        "SCHEMA_CREATE",
        "SCHEMA_GET",
        "SCHEMA_GET_ALL",
        "SCHEMA_DELETE",

        "NODE_CREATE",
        "NODE_UPDATE",
        "NODE_DELETE",
        "NODE_MATCH",

        "LINK_CREATE",
        "LINK_DELETE",

        "MATCH"
};

const char *query_result_types[] = {
        "RESULT_SCHEMA",
        "RESULT_GENERATOR",
        "RESULT_NONE"
};

const char *predicate_types[] = {
        "PT_TERM",
        "PT_OR",
        "PT_AND"
};

const char *comparable_types[] = {
        "CMPT_FIELD",
        "CMPT_VALUE",
};

const char *cmp_types[] = {
        "CMP_GT",
        "CMP_LT",
        "CMP_GE",
        "CMP_LE",
        "CMP_EQ",
        "CMP_NEQ",
        "CMP_CONTAINS"
};

const char *link_types[] = {
        "LT_TO",
        "LT_FROM",
        "LT_BOTH",

        "LT_ANY",
};

void print_tabs(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("    ");
    }
}

void print_value_value(Value *value) {
    switch (value->vt) {
        case VT_STRING:
            printf("%s", value->string);
            break;
        case VT_INTEGER:
            printf("%d", value->integer);
            break;
        case VT_FLOATING:
            printf("%f", value->floating);
            break;
        case VT_BOOLEAN:
            printf(value->boolean ? "true": "false");
            break;
    }
}

void print_value(Value *value, int depth) {
    printf("{\n");
    print_tabs(depth + 1);
    printf("type: %s,\n", value_types[value->vt]);
    print_tabs(depth + 1);
    printf("value: ");
    print_value_value(value);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

void print_attribute(Attribute *attribute, int depth) {
    printf("{\n");
    print_tabs(depth + 1);
    printf("name: %s,\n", attribute->name);
    print_tabs(depth + 1);
    printf("type: %s\n", value_types[attribute->vt]);
    print_tabs(depth);
    printf("}");
}

void print_property(Property *property, int depth) {
    printf("{\n");
    print_tabs(depth + 1);
    printf("name: %s,\n", property->field);
    print_tabs(depth + 1);
    printf("value: ");
    print_value(property->value, depth + 1);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

#define PRINT_LIST(Type, list, print_fun, depth)    \
do {                                                \
printf("[\n");                                      \
Type *x = list;                                     \
while (x != NULL) {                                 \
    print_tabs((depth) + 2);                        \
    print_fun(x, (depth) + 2);                      \
    if (x->next != NULL)                            \
        printf(",");                                \
    printf("\n");                                   \
    x = x->next;                                    \
}                                                   \
print_tabs((depth) + 1);                            \
printf("]");                                        \
} while (0)                                         \

void print_schema(Schema *schema, int depth) {
    printf("{\n");
    print_tabs(depth + 1);
    printf("name: %s,\n", schema->name);
    print_tabs(depth + 1);
    printf("attrs: ");
    PRINT_LIST(Attribute, schema->attributes, print_attribute, depth);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

void print_node(Node *node, int depth) {
    printf("{\n");
    print_tabs(depth + 1);
    printf("name: %s,\n", node->name);
    print_tabs(depth + 1);
    printf("props: ");
    PRINT_LIST(Property, node->properties, print_property, depth);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

void print_comparable(Comparable *comparable, int depth) {
    printf("{\n");
    print_tabs(depth + 1);
    printf("type: %s,\n", comparable_types[comparable->type]);
    print_tabs(depth + 1);
    switch (comparable->type) {
        case CMPT_FIELD:
            printf("field_name: %s", comparable->field_name);
            break;
        case CMPT_VALUE:
            printf("value: ");
            print_value(comparable->value, depth + 1);
            break;
    }
    printf("\n");
    print_tabs(depth);
    printf("}");
}

void print_compare(Compare *compare, int depth) {
    printf("{\n");
    print_tabs(depth + 1);
    printf("cmp: %s,\n", cmp_types[compare->cmp]);
    print_tabs(depth + 1);
    printf("with: ");
    print_comparable(compare->with, depth + 1);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

void print_condition_compare(ConditionCompare *term, int depth) {
    printf("{\n");
    print_tabs(depth + 1);
    printf("left: ");
    print_comparable(term->left, depth + 1);
    printf(",\n");
    print_tabs(depth + 1);
    printf("compare: ");
    print_compare(term->compare, depth + 1);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

void print_predicate(Predicate *predicate, int depth) {
    printf("{\n");
    print_tabs(depth + 1);
    printf("type: %s,\n", predicate_types[predicate->type]);
    print_tabs(depth + 1);
    switch (predicate->type) {
        case PT_TERM:
            printf("term: ");
            print_condition_compare(predicate->term, depth + 1);
            break;
        case PT_OR:
            printf("or: ");
            PRINT_LIST(Predicate, predicate->or, print_predicate, depth);
            break;
        case PT_AND:
            printf("and: ");
            PRINT_LIST(Predicate, predicate->and, print_predicate, depth);
            break;
    }
    printf("\n");
    print_tabs(depth);
    printf("}");
}

char *bool_str(bool b) {
    return b ? "true" : "false";
}

void print_node_condition(NodeCondition *cond, int depth) {
    if (cond == NULL) {
        printf("(null)");
        return;
    }

    printf("{\n");
    print_tabs(depth + 1);
    printf("is_null: %s,\n", bool_str(cond->is_null));
    print_tabs(depth + 1);
    printf("schema: %s,\n", cond->schema);
    print_tabs(depth + 1);
    printf("predicates: ");
    PRINT_LIST(Predicate, cond->predicates, print_predicate, depth);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

void print_link(Link *link, int depth) {
    printf("{\n");
    print_tabs(depth + 1);
    printf("name: %s,\n", link->name);
    print_tabs(depth + 1);
    printf("type: %s,\n", link_types[link->type]);
    print_tabs(depth + 1);
    printf("first: %s,\n", link->first);
    print_tabs(depth + 1);
    printf("second: %s,\n", link->second);
    print_tabs(depth);
    printf("}");
}

void print_link_condition(LinkCondition *cond, int depth) {
    if (cond == NULL) {
        printf("(null)");
        return;
    }

    printf("{\n");
    print_tabs(depth + 1);
    printf("is_null: %s,\n", bool_str(cond->is_null));
    print_tabs(depth + 1);
    printf("link_type: %s,\n", link_types[cond->link_type]);
    print_tabs(depth + 1);
    printf("link_name: %s\n", cond->link_name);
    print_tabs(depth);
    printf("}");
}

void print_match_condition(MatchCondition *condition, int depth) {
    printf("{\n");
    print_tabs(depth + 1);
    printf("node_first_condition: ");
    print_node_condition(condition->node_first_condition, depth + 1);
    printf(",\n");
    print_tabs(depth + 1);
    printf("node_second_condition: ");
    print_node_condition(condition->node_second_condition, depth + 1);
    printf(",\n");
    print_tabs(depth + 1);
    printf("node_cross_condition: ");
    print_node_condition(condition->node_cross_condition, depth + 1);
    printf(",\n");
    print_tabs(depth + 1);
    printf("link_condition: ");
    print_link_condition(condition->link_condition, depth + 1);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

void print_schema_create_query(Query _query, int depth) {
    SchemaCreateQuery query = _query.schema_create_query;

    printf("{\n");
    print_tabs(depth + 1);
    printf("schema: ");
    print_schema(query.schema, depth + 1);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

void print_schema_get_query(Query _query, int depth) {
    SchemaGetQuery query = _query.schema_get_query;

    printf("{\n");
    print_tabs(depth + 1);
    printf("schema_name: %s\n", query.schema_name);
    print_tabs(depth);
    printf("}");
}

void print_schema_get_all_query(Query query, int depth) {
    printf("{}");
}

void print_schema_delete_query(Query _query, int depth) {
    SchemaDeleteQuery query = _query.schema_delete_query;

    printf("{\n");
    print_tabs(depth + 1);
    printf("schema_name: %s\n", query.schema_name);
    print_tabs(depth);
    printf("}");
}

void print_node_create_query(Query _query, int depth) {
    NodeCreateQuery query = _query.node_create_query;

    printf("{\n");
    print_tabs(depth + 1);
    printf("schema_name: %s,\n", query.schema_name);
    print_tabs(depth + 1);
    printf("node: ");
    print_node(query.node, depth + 1);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

void print_node_update_query(Query _query, int depth) {
    NodeUpdateQuery query = _query.node_update_query;

    printf("{\n");
    print_tabs(depth + 1);
    printf("node_condition: ");
    print_node_condition(query.condition, depth + 1);
    printf(",\n");
    print_tabs(depth + 1);
    printf("properties: ");
    PRINT_LIST(Property, query.properties, print_property, depth);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

void print_node_delete_query(Query _query, int depth) {
    NodeDeleteQuery query = _query.node_delete_query;

    printf("{\n");
    print_tabs(depth + 1);
    printf("node_condition: ");
    print_node_condition(query.condition, depth + 1);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

void print_node_match_query(Query _query, int depth) {
    NodeMatchQuery query = _query.node_match_query;

    printf("{\n");
    print_tabs(depth + 1);
    printf("node_condition: ");
    print_node_condition(query.condition, depth + 1);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

void print_link_create_query(Query _query, int depth) {
    LinkCreateQuery query = _query.link_create_query;

    printf("{\n");
    print_tabs(depth + 1);
    printf("name: %s,\n", query.name);
    print_tabs(depth + 1);
    printf("link_type: %s,\n", link_types[query.link_type]);
    print_tabs(depth + 1);
    printf("first: ");
    print_node_condition(query.first, depth + 1);
    printf(",\n");
    print_tabs(depth + 1);
    printf("second: ");
    print_node_condition(query.second, depth + 1);
    printf(",\n");
    print_tabs(depth);
    printf("}");
}

void print_link_delete_query(Query _query, int depth) {
    LinkDeleteQuery query = _query.link_delete_query;

    printf("{\n");
    print_tabs(depth + 1);
    printf("match_query: ");
    print_match_condition(query.condition, depth + 1);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

void print_match_query(Query _query, int depth) {
    MatchQuery query = _query.match_query;

    printf("{\n");
    print_tabs(depth + 1);
    printf("match_query: ");
    print_match_condition(query.condition, depth + 1);
    printf("\n");
    print_tabs(depth);
    printf("}");
}

#define PRINTERS_NUM (MATCH + 1)

void (*printers[PRINTERS_NUM])(Query, int) = {
        &print_schema_create_query,
        &print_schema_get_query,
        &print_schema_get_all_query,
        &print_schema_delete_query,
        &print_node_create_query,
        &print_node_update_query,
        &print_node_delete_query,
        &print_node_match_query,
        &print_link_create_query,
        &print_link_delete_query,
        &print_match_query,
};

const QueryResultType result_types[] = {
        RESULT_NONE,
        RESULT_SCHEMA,
        RESULT_GENERATOR,
        RESULT_NONE,

        RESULT_NONE,
        RESULT_NONE,
        RESULT_NONE,
        RESULT_GENERATOR,

        RESULT_NONE,
        RESULT_NONE,

        RESULT_GENERATOR
};

void print_query(Query query) {
    int depth = 0;
    printf("{\n");
    print_tabs(++depth);
    printf("query_type: %s,\n", query_types[query.type]);
    print_tabs(depth);
    printf("result_type: %s,\n", query_result_types[result_types[query.type]]);
    print_tabs(depth);
    printf("query: ");
    printers[query.type](query, depth);
    printf("\n}\n");
}
