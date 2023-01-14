#include "server_convert.h"

#define CONVERT_ARRAY(Item, item) \
Item **convert_##item##_list(GPtrArray *array) { \
    Item **item = malloc(array->len * sizeof(Item)); \
    for (size_t i = 0; i < array->len; i++) \
        item[i] = convert_##item(array->pdata[i]); \
    return item; \
}

String *convert_string(char *string) {
    String *result = string_init();
    result->size = strlen(string);
    result->content = malloc(result->size * sizeof(char));
    memcpy(result->content, string, result->size);
    return result;
}

Value *convert_value(I_Value *i_value) {
    Value *value = value_init();

    I_ValueType type;

    g_object_get(i_value,
                 "type", &type,
                 NULL);
    I_uValue *u_value = i_value->value;

    value->data_type = (DataType)type;

    switch (type) {
        case I__VALUE_TYPE_VT_INTEGER:
            value->integer = u_value->integer;
            break;
        case I__VALUE_TYPE_VT_STRING:
            value->string = convert_string(u_value->string_);
            break;
        case I__VALUE_TYPE_VT_FLOATING:
            value->floating = (float) u_value->floating;
            break;
        case I__VALUE_TYPE_VT_BOOLEAN:
            value->boolean = u_value->boolean;
            break;
    }

    return value;
}

Attribute *convert_attribute(I_Attribute *i_attribute) {
    gchar *name;
    I_ValueType vt;
    g_object_get(i_attribute,
                 "name", &name,
                 "vt", &vt,
                 NULL);

    Attribute *attribute = attribute_init();
    attribute->name = convert_string(name);
    attribute->data_type = (int) vt;

    return attribute;
}

CONVERT_ARRAY(Attribute, attribute)

Value *convert_property(I_Property *property) {
    I_Value *value = g_object_new(TYPE_I__VALUE, NULL);
    g_object_get(property,
                 "value", &value,
                 NULL);
    return convert_value(value);
}

Value **convert_property_list(GPtrArray *array) {
    Value **property = malloc(array->len * sizeof(Value));
    for (size_t i = 0; i < array->len; i++)
        property[i] = convert_property(array->pdata[i]);
    return property;
}

Schema *convert_schema(I_Schema *i_schema) {
    gchar *name;
    GPtrArray *array;

    Schema *schema = schema_init();
    g_object_get(i_schema,
                 "name", &name,
                 "attributes", &array,
                 NULL);
    schema->name = convert_string(name);
    schema->attr_count = array->len;
    schema->attributes = convert_attribute_list(array);
    return schema;
}

Node *convert_node(I_Node *i_node) {
    Node *node = node_init();

    GPtrArray *properties;
    g_object_get(i_node,
                 "properties", &properties,
                 NULL);

    node->value_count = properties->len;
    node->values = convert_property_list(properties);
    return node;
}

SchemaCreateQuery convert_schema_create_query(I_SchemaCreateQuery *query) {
    I_Schema *schema;
    g_object_get(query,
                 "schema", &schema,
                 NULL);
    return (SchemaCreateQuery) {.schema = convert_schema(schema)};
}

SchemaGetQuery convert_schema_get_query(I_SchemaGetQuery *query) {
    gchar *schema_name;
    g_object_get(query,
                 "schema_name", &schema_name,
                 NULL);
    return (SchemaGetQuery) {.schema_name = schema_name};
}

SchemaDeleteQuery convert_schema_delete_query(I_SchemaDeleteQuery *query) {
    gchar *schema_name;
    g_object_get(query,
                 "schema_name", &schema_name,
                 NULL);
    return (SchemaDeleteQuery) {.schema_name = schema_name};
}

NodeCreateQuery convert_node_create_query(I_NodeCreateQuery *query) {
    gchar *schema_name;
    I_Node *i_node = g_object_new(TYPE_I__NODE, NULL);
    g_object_get(query,
                 "schema_name", &schema_name,
                 "node", &i_node,
                 NULL);
    return (NodeCreateQuery) {
            .schema_name = schema_name,
            .node = convert_node(i_node)
    };
}

ConditionCompare convert_predicate(I_Predicate *predicate) {
    ConditionCompare result = {};

    I_uPredicate *u_predicate = predicate->predicate;
    I_ConditionCompare *i_condition_compare = u_predicate->term;

    I_Comparable *comparable = i_condition_compare->left;
    I_Compare *compare = i_condition_compare->compare;

    I_ComparableType first = comparable->type;
    I_uComparable *left_comparable = comparable->comparable;

    I_Comparation cmp = compare->cmp;
    I_Comparable *right = compare->with_;

    I_uComparable *right_comparable = right->comparable;
    I_ComparableType second = right->type;

    result.left = (Comparable) {.type = (int)first};
    if (result.left.type == CMPT_VALUE) {
        I_Value *i_value;
        g_object_get(left_comparable,
                     "value", &i_value,
                     NULL);
        result.left.value = convert_value(i_value);
    } else {
        g_object_get(left_comparable,
                     "field_name", &result.left.field_name,
                     NULL);
    }
    switch (cmp) {
        case I__COMPARATION_CMP_GT:
        case I__COMPARATION_CMP_GE:
            result.cmp = CMP_GREATER;
            break;
        case I__COMPARATION_CMP_LT:
        case I__COMPARATION_CMP_LE:
            result.cmp = CMP_LESS;
            break;
        case I__COMPARATION_CMP_EQ:
            result.cmp = CMP_EQUAL;
            break;
        case I__COMPARATION_CMP_CONTAINS:
        case I__COMPARATION_CMP_NEQ:
            result.cmp = CMP_UNDEFINED;
            break;
    }
    result.right = (Comparable) {.type = (int)second};
    if (result.right.type == CMPT_VALUE) {
        I_Value *i_value;
        g_object_get(right_comparable,
                     "value", &i_value,
                     NULL);
        result.right.value = convert_value(i_value);
    } else {
        g_object_get(right_comparable,
                     "field_name", &result.right.field_name,
                     NULL);
    }

    return result;
}

NodeCondition convert_node_condition(I_NodeCondition *condition) {
    NodeCondition node_condition = {0};
    GPtrArray *predicates;
    g_object_get(condition,
                 "is_null", &node_condition.is_null,
                 "schema", &node_condition.schema_name,
                 "predicates", &predicates,
                 NULL);

    if (predicates->len == 0) {
        node_condition.conditions_num = 0;
        return node_condition;
    }

    I_Predicate *pred = predicates->pdata[0];

    if (pred->type == I__PREDICATE_TYPE_PT_TERM) {
        ConditionCompare *p_compare = malloc(sizeof(ConditionCompare));
        p_compare[0] = convert_predicate(pred);
        node_condition.compare_conditions = p_compare;
        node_condition.conditions_num = 1;
    } else if (pred->type == I__PREDICATE_TYPE_PT_AND) {
        GPtrArray *ands = pred->predicate->and_;
        ConditionCompare *p_compare = malloc(ands->len * sizeof(ConditionCompare));
        for (size_t i = 0; i < ands->len; i++)
            p_compare[i] = convert_predicate(ands->pdata[i]);
        node_condition.compare_conditions = p_compare;
        node_condition.conditions_num = ands->len;
    }
    return node_condition;
}

NodeMatchQuery convert_node_match_query(I_NodeMatchQuery *query) {
    I_NodeCondition *condition;
    g_object_get(query,
                 "condition", &condition,
                 NULL);

    NodeCondition node_condition = convert_node_condition(condition);
    return (NodeMatchQuery) {
        .condition = node_condition
    };
}

NodeUpdateQuery convert_node_update_query(I_NodeUpdateQuery *query) {
    I_NodeCondition *condition;
    GPtrArray *properties;
    g_object_get(query,
                 "condition", &condition,
                 "properties", &properties,
                 NULL);

    char **attributes_to_update = malloc(properties->len * sizeof(char*));
    Value **values = malloc(properties->len * sizeof(Value*));

    for (size_t i = 0; i < properties->len; i++) {
        gchar *field;
        I_Value *value;
        g_object_get(properties->pdata[i],
                     "field", &field,
                     "value", &value,
                     NULL);
        attributes_to_update[i] = g_strdup(field);
        values[i] = convert_value(value);
    }

    return (NodeUpdateQuery) {
        .condition = convert_node_condition(condition),
        .attributes_to_update_number = properties->len,
        .attributes_to_update = attributes_to_update,
        .new_values = values
    };
}

NodeDeleteQuery convert_node_delete_query(I_NodeDeleteQuery *query) {
    I_NodeCondition *condition;
    g_object_get(query,
                 "condition", &condition,
                 NULL);
    return (NodeDeleteQuery) {
            .condition = convert_node_condition(condition)
    };
}

LinkCondition convert_link_condition(I_LinkCondition *condition) {
    String *link_name = string_init();
    link_name->size = strlen(condition->link_name);
    link_name->content = strndup(condition->link_name, link_name->size);
    return (LinkCondition) {
        .is_null = condition->is_null,
        .link_name = link_name,
        .link_type = (LinkType) condition->link_type
    };
}

MatchCondition convert_match_condition(I_MatchCondition *condition) {
    return (MatchCondition) {
        .is_null = condition->is_null,
        .link_condition = convert_link_condition(condition->link_condition),
        .node_first_condition = convert_node_condition(condition->node_first_condition),
        .node_second_condition = convert_node_condition(condition->node_second_condition),
        .node_cross_condition = convert_node_condition(condition->node_cross_condition),
    };
}

LinkCreateQuery convert_link_create_query(I_LinkCreateQuery *query) {
    Link *link = link_init();
    link->name = string_init();
    link->name->size = strlen(query->link_name);
    link->name->content = strndup(query->link_name, link->name->size);
    link->type = (LinkType) query->link_type;


    return (LinkCreateQuery) {
        .link = link,
        .first = convert_node_condition(query->first),
        .second = convert_node_condition(query->second)
    };
}

LinkDeleteQuery convert_link_delete_query(I_LinkDeleteQuery *query) {
    return (LinkDeleteQuery) {
        .condition = convert_match_condition(query->condition)
    };
}

MatchQuery convert_match_query(I_MatchQuery *query) {
    return (MatchQuery) {
        .condition = convert_match_condition(query->condition),
        .return_items = RETURN_ALL
    };
}

Query convert_query(I_Query *query) {
    I_uQuery *u_query;
    g_object_get(query,
                 "query", &u_query,
                 NULL);

    I_SchemaCreateQuery *schema_create_query;
    I_SchemaGetQuery *schema_get_query;
    I_SchemaDeleteQuery *schema_delete_query;

    I_NodeCreateQuery *node_create_query;
    I_NodeUpdateQuery *node_update_query;
    I_NodeDeleteQuery *node_delete_query;
    I_NodeMatchQuery *node_match_query;

    I_LinkCreateQuery *link_create_query;
    I_LinkDeleteQuery *link_delete_query;
    I_MatchQuery *match_query;

    Query result = {0};
    g_object_get(query,
                 "type", &result.type,
                 NULL);
    switch (result.type) {
        case SCHEMA_CREATE:
            g_object_get(u_query,
                         "schema_create_query", &schema_create_query,
                         NULL);
            result.schema_create_query = convert_schema_create_query(schema_create_query);
            break;
        case SCHEMA_GET:
            g_object_get(u_query,
                         "schema_get_query", &schema_get_query,
                         NULL);
            result.schema_get_query = convert_schema_get_query(schema_get_query);
            break;
        case SCHEMA_GET_ALL:
            result.type = SCHEMA_GET_ALL;
            break;
        case SCHEMA_DELETE:
            g_object_get(u_query,
                         "schema_delete_query", &schema_delete_query,
                         NULL);
            result.schema_delete_query = convert_schema_delete_query(schema_delete_query);
            break;
        case NODE_CREATE:
            g_object_get(u_query,
                         "node_create_query", &node_create_query,
                         NULL);
            result.node_create_query = convert_node_create_query(node_create_query);
            break;
        case NODE_UPDATE:
            g_object_get(u_query,
                         "node_update_query", &node_update_query,
                         NULL);
            result.node_update_query = convert_node_update_query(node_update_query);
            break;
        case NODE_DELETE:
            g_object_get(u_query,
                         "node_delete_query", &node_delete_query,
                         NULL);
            result.node_delete_query = convert_node_delete_query(node_delete_query);
            break;
        case NODE_MATCH:
            g_object_get(u_query,
                         "node_match_query", &node_match_query,
                         NULL);
            result.node_match_query = convert_node_match_query(node_match_query);
            break;
        case LINK_CREATE:
            g_object_get(u_query,
                         "link_create_query", &link_create_query,
                         NULL);
            result.link_create_query = convert_link_create_query(link_create_query);
            break;
        case LINK_UPDATE:
            // Not implemented. Use delete + create
            break;
        case LINK_DELETE:
            g_object_get(u_query,
                         "link_delete_query", &link_delete_query,
                         NULL);
            result.link_delete_query = convert_link_delete_query(link_delete_query);
            break;
        case MATCH:
            g_object_get(u_query,
                         "match_query", &match_query,
                         NULL);
            result.match_query = convert_match_query(match_query);
            break;
    }
    return result;
}

I_Attribute *get_attribute(Attribute *attribute) {
    gchar *name[attribute->name->size + 1];
    memcpy(name, attribute->name->content, attribute->name->size);
    name[attribute->name->size] = '\0';
    I_Attribute *i_attribute = g_object_new(
            TYPE_I__ATTRIBUTE,
            "name", g_strdup(attribute->name->content),
            "vt", attribute->data_type,
            NULL);
    return i_attribute;
}

I_Schema *get_schema(Schema *schema) {
    I_Schema *i_schema = g_object_new(TYPE_I__SCHEMA, NULL);
    GPtrArray *array = g_ptr_array_new();
    gchar *name = g_strndup(schema->name->content, schema->name->size);
    g_object_set(i_schema,
                 "name", name,
                 "attributes", array,
                 NULL);
    for (size_t i = 0; i < schema->attr_count; i++)
        g_ptr_array_add(array, get_attribute(schema->attributes[i]));
    return i_schema;
}

I_Value *get_value(Value *value) {
    I_uValue *u_value = g_object_new(TYPE_I_U_VALUE, NULL);

    switch (value->data_type) {
        case DT_INTEGER:
            g_object_set(u_value,
                         "integer", value->integer,
                         NULL);
            break;
        case DT_STRING:
            g_object_set(u_value,
                         "string_", g_strndup(value->string->content, value->string->size),
                         NULL);
            break;
        case DT_FLOATING:
            g_object_set(u_value,
                         "floating", value->floating,
                         NULL);
            break;
        case DT_BOOLEAN:
            g_object_set(u_value,
                         "boolean", value->boolean,
                         NULL);
            break;
    }

    I_Value *result = g_object_new(TYPE_I__VALUE,
                                   "type", value->data_type,
                                   "value", u_value,
                                   NULL);

    return result;
}

I_Node *get_node(Node *node) {
    I_Node *i_node = g_object_new(TYPE_I__NODE, NULL);

    GPtrArray *values = g_ptr_array_new();
    for (size_t i = 0; i < node->value_count; i++) {
        I_Value *value = get_value(node->values[i]);
        I_Property *i_property = g_object_new(TYPE_I__PROPERTY,
                                              "value", value,
                                              NULL);
        g_ptr_array_add(values, i_property);
    }

    g_object_set(i_node,
                 "properties", values,
                 NULL);

    return i_node;
}

I_Link *get_link(Link *link) {
    I_Link *i_link = g_object_new(
            TYPE_I__LINK,
            "name", g_strndup(link->name->content, link->name->size),
            "type", (I_LinkType) link->type,
            NULL);
    return i_link;
}


GPtrArray *get_items(Generator *generator, QueryType q_type) {
    GPtrArray *items = g_ptr_array_new();
    I_Result *i_result;

    if (q_type == SCHEMA_GET_ALL) {
        Schema *schema;
        while ((schema = generator_next(generator)) != NULL) {
            i_result = g_object_new(TYPE_I__RESULT, NULL);
            g_object_set(i_result,
                         "schema", get_schema(schema),
                         NULL);
            g_ptr_array_add(items, i_result);
        }
    } else if (q_type == NODE_MATCH) {
        Node *node;
        while ((node = generator_next(generator)) != NULL) {
            i_result = g_object_new(TYPE_I__RESULT, NULL);
            g_object_set(i_result,
                         "first", get_node(node),
                         NULL);
            g_ptr_array_add(items, i_result);
        }
    } else if (q_type == MATCH) {
        Result *result;
        while ((result = generator_next(generator)) != NULL) {
            i_result = g_object_new(TYPE_I__RESULT, NULL);
            g_object_set(i_result,
                         "first", get_node(result->first),
                         "second", get_node(result->second),
                         "link", get_link(result->link),
                         NULL);
        }
    }

    return items;
}

I_QueryResult *convert_result(QueryResult query_result, QueryType q_type) {
    char message[1024];
    sprintf(message,
            "Result code: %d (%s)",
            query_result.status, RESULT_CODES[query_result.status]);

    I_QueryResult *result = g_object_new(TYPE_I__QUERY_RESULT, NULL);
    I_uQueryResult *u_query = g_object_new(TYPE_I_U_QUERY_RESULT, NULL);
    g_object_set(result,
                 "type", query_result.type,
                 "message", g_strdup(message),
                 "result", u_query,
                 NULL);

    if (q_type == I__QUERY_TYPE_SCHEMA_GET) {
        g_object_set(u_query,
                     "schema", get_schema(query_result.schema),
                     NULL);
    } else if (q_type == I__QUERY_TYPE_NODE_MATCH || q_type == I__QUERY_TYPE_MATCH || q_type == I__QUERY_TYPE_SCHEMA_GET_ALL){
        Generator *generator = query_result.generator;
        GPtrArray *items = get_items(generator, q_type);

        g_object_set(u_query,
                     "items", items,
                     NULL);
    }

    return result;
}