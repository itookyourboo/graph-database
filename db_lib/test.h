#ifndef LOW_LEVEL_PROGRAMMING_TASK1_TEST_H
#define LOW_LEVEL_PROGRAMMING_TASK1_TEST_H

#include <time.h>
#include "database/query_manager.h"
#include "database/visualize.h"


#define RANGE_SCHEMA_NAME 4, 20
#define RANGE_ATTR_NAME 2, 15
#define RANGE_ATTR_COUNT 2, 8
#define RANGE_VALUE_STRING 2, 10
#define RANGE_LINK_NAME 4, 8

#define TEST_SCHEMAS_COUNT 5
#define TEST_NODES_IN_SCHEMA_COUNT 20
#define TEST_LINKS_COUNT 50

#define TEST_SCHEMA_NAME "students"
#define TEST_SCHEMA_ATTR_COUNT 4
#define TEST_SCHEMA_SIZE 30
#define TEST_SCHEMA_LINKS 20


volatile bool print = false;


size_t randint(size_t size_from, size_t size_to) {
    return rand() % (size_to - size_from + 1) + size_from;
}

DataType get_random_date_type() {
    return rand() % 4;
}

String *get_random_string(size_t size_from, size_t size_to) {
    size_t str_len = randint(size_from, size_to);

    char alphabet[27] = "abcdefghijklmnopqrstuvwxyz_";

    String *res = string_init();
    res->size = str_len;
    res->content = malloc(res->size * sizeof(*res->content));

    for (size_t j = 0; j < str_len; j++)
        res->content[j] = alphabet[rand() % 27];

    return res;
}

Value *get_random_value(DataType data_type) {
    Value *value = value_init();
    value->data_type = data_type;
    switch (data_type) {
        case DT_INTEGER:
            value->integer = rand();
            break;
        case DT_BOOLEAN:
            value->boolean = rand() % 2;
            break;
        case DT_FLOATING:
            value->floating = (dt_floating) rand() / (dt_floating) rand();
            break;
        case DT_STRING:
            value->string = get_random_string(RANGE_VALUE_STRING);
            break;
    }
    return value;
}

Attribute *get_random_attribute() {
    Attribute *attr = attribute_init();
    attr->name = get_random_string(RANGE_ATTR_NAME);
    attr->data_type = get_random_date_type();
    return attr;
}

void insert_random_schema(Connection *connection) {
    Schema *schema = schema_init();
    schema->name = get_random_string(RANGE_SCHEMA_NAME);
    schema->attr_count = randint(RANGE_ATTR_COUNT);

    Attribute **attributes = malloc(schema->attr_count * sizeof(*schema->attributes));
    for (size_t i = 0; i < schema->attr_count; i++)
        attributes[i] = get_random_attribute();
    schema->attributes = attributes;

    query_execute((Query) {
            .type = SCHEMA_CREATE,
            .schema_create_query = {
                    .schema = schema
            }}, connection);

    if (print) {
        printf("\t> ");
        string_print(schema->name);
        printf("\n");
    }

    schema_free(schema);
}

void insert_test_schema(Connection *connection) {
    Schema schema = {0};
    schema.name = &(String) {8, TEST_SCHEMA_NAME};
    schema.attr_count = TEST_SCHEMA_ATTR_COUNT;

    schema.attributes = (Attribute *[TEST_SCHEMA_ATTR_COUNT]) {
            &(Attribute) {.data_type = DT_INTEGER, .name = &(String) {2, "id"}},
            &(Attribute) {.data_type = DT_STRING, .name = &(String) {4, "name"}},
            &(Attribute) {.data_type = DT_FLOATING, .name = &(String) {9, "avg_score"}},
            &(Attribute) {.data_type = DT_BOOLEAN, .name = &(String) {7, "is_male"}}
    };

    query_execute((Query) {
            .type = SCHEMA_CREATE,
            .schema_create_query = {
                    .schema = &schema
            }}, connection);

    if (print) {
        printf("\t> ");
        schema_print(&schema);
        printf("\n");
    }
}

void insert_random_node(Schema *schema, Connection *connection) {
    Node *node = node_init();
    node->value_count = schema->attr_count;
    node->values = malloc(node->value_count * sizeof(node->values));

    for (size_t i = 0; i < node->value_count; i++)
        node->values[i] = get_random_value(schema->attributes[i]->data_type);

    query_execute((Query) {
            .type = NODE_CREATE,
            .node_create_query = (NodeCreateQuery) {
                    .schema = schema,
                    .node = node
            }
    }, connection);

    if (print) {
        printf("\t> ");
        string_print(schema->name);
        printf(": ");
        node_print(node);
        printf("\n");
    }

    node_free(node);
}

void insert_random_link(Node *node1, Node *node2, Connection *connection) {
    Link *link = link_init();
    link->type = rand() % 3;
    link->name = get_random_string(RANGE_LINK_NAME);

    query_execute((Query) {
            .type = LINK_CREATE,
            .link_create_query = (LinkCreateQuery) {
                    .link = link,
                    .first = node1,
                    .second = node2
            }
    }, connection);

    if (print) {
        printf("\t ");
        link_print(link);
        printf("\n");
    }

    link_free(link);
}

void insert_test_schemas(size_t count, Connection *connection) {
    for (size_t i = 0; i < count; i++)
        insert_random_schema(connection);
}

void insert_test_nodes(size_t count, Connection *connection) {
    QueryResult qr = query_execute((Query) {.type = SCHEMA_GET_ALL}, connection);
    SchemaVector *schema_vector = schema_vector_fetch_all(qr.generator);
    V_ITERATE(schema_vector, i)
        for (size_t j = 0; j < count; j++)
            insert_random_node(schema_vector_get(i, schema_vector), connection);
    schema_vector_free(schema_vector);
}

void insert_test_links(size_t count, Connection *connection) {
    QueryResult qr = query_execute((Query) {.type = NODE_MATCH, .node_match_query = (NodeMatchQuery) {
            .condition = NULL,
    }}, connection);
    NodeVector *node_vector = node_vector_fetch_all(qr.generator);
    size_t size = node_vector_get_size(node_vector);
    for (size_t i = 0; i < count; i++)
        insert_random_link(
                node_vector_get(rand() % size, node_vector),
                node_vector_get(rand() % size, node_vector),
                connection
        );
    node_vector_free(node_vector);
}

void test_prepare(Connection *connection) {
    printf("> Inserting test schemas\n");
    insert_test_schemas(TEST_SCHEMAS_COUNT, connection);
    printf("> Inserting test nodes\n");
    insert_test_nodes(TEST_NODES_IN_SCHEMA_COUNT, connection);
    printf("> Inserting test links\n");
    insert_test_links(TEST_LINKS_COUNT, connection);
}

void test_manipulate(Connection *connection) {
    printf("> Inserting test schema\n");
    insert_test_schema(connection);

    Query GET_SCHEMA = (Query) {.type = SCHEMA_GET, .schema_get_query = (SchemaGetQuery) {.schema_name = TEST_SCHEMA_NAME}};
    QueryResult qr = query_execute(GET_SCHEMA, connection);
    Schema *schema = qr.schema;
    schema_print(schema);

    for (size_t i = 0; i < TEST_SCHEMA_SIZE; i++)
        insert_random_node(schema, connection);

    insert_test_links(TEST_SCHEMA_LINKS, connection);

    Query GET_ALL_NODES = (Query) {
            .type = NODE_MATCH,
            .node_match_query = (NodeMatchQuery) {
                    .condition = {.is_null = true},
            }
    };
    printf("All nodes:\n");
    qr = query_execute(GET_ALL_NODES, connection);
    NodeVector *node_vector = node_vector_fetch_all(qr.generator);
    V_ITERATE(node_vector, i) {
        node_print(node_vector_get(i, node_vector));
        printf("\n");
    }

    NodeCondition MATCH_CONDITION = {
            .schema = schema,
            .conditions_num = 2,
            .compare_conditions = (ConditionCompare[2]) {
                    {
                            .left = {.type = CMPT_FIELD_NAME, .field_name = "id"},
                            .cmp = CMP_GREATER,
                            .right = {.type = CMPT_VALUE, .value = &(Value) {
                                    .data_type = DT_INTEGER, .integer = 1189641421
                            }}
                    },
                    {
                            .left = {.type = CMPT_FIELD_NAME, .field_name = "name"},
                            .cmp = CMP_LESS,
                            .right = {.type = CMPT_VALUE, .value = &(Value) {
                                    .data_type = DT_STRING, .string = &(String) {7, "naaaaaa"}
                            }}
                    }
            }};

    printf("Nodes with condition:\n");
    Query GET_NODES_WITH_CONDITION = (Query) {
            .type = NODE_MATCH,
            .node_match_query = (NodeMatchQuery) {
                    .condition = MATCH_CONDITION
            }
    };
    qr = query_execute(GET_NODES_WITH_CONDITION, connection);
    node_vector = node_vector_fetch_all(qr.generator);
    V_ITERATE(node_vector, i) node_print(node_vector_get(i, node_vector));
    printf("\n");

    Query UPDATE_NODES = (Query) {
            .type = NODE_UPDATE,
            .node_update_query = (NodeUpdateQuery) {
                    .schema = schema,
                    .attributes_to_update_number = 1,
                    .attributes_to_update = (char *[1]) {"name"},
                    .new_values = (Value *[1]) {&(Value) {.data_type = DT_STRING, .string = &(String) {7, "abobusp"}}},
                    .node_vector = node_vector
            }
    };

    printf("After update:\n");
    qr = query_execute(UPDATE_NODES, connection);

    qr = query_execute(GET_NODES_WITH_CONDITION, connection);
    node_vector = node_vector_fetch_all(qr.generator);
    V_ITERATE(node_vector, i) node_print(node_vector_get(i, node_vector));
    printf("\n");

    Query DELETE_NODES = (Query) {
            .type = NODE_DELETE,
            .node_delete_query = (NodeDeleteQuery) {
                    .schema = schema,
                    .node_vector = node_vector
            }
    };
    printf("After delete:\n");
    qr = query_execute(DELETE_NODES, connection);

    printf("Should be deleted:\n");
    qr = query_execute(GET_NODES_WITH_CONDITION, connection);
    node_vector = node_vector_fetch_all(qr.generator);
    V_ITERATE(node_vector, i) node_print(node_vector_get(i, node_vector));

    printf("\nAll nodes after delete:\n");

    qr = query_execute(GET_ALL_NODES, connection);
    node_vector = node_vector_fetch_all(qr.generator);
    V_ITERATE(node_vector, i) node_print(node_vector_get(i, node_vector));

    String link_names[2] = {{5, "LOVES"},
                            {5, "HATES"}};
    for (int i = 0; i < TEST_LINKS_COUNT; ++i) {
        Query GET_ALL_LINKS = (Query) {
                .type = LINK_CREATE,
                .link_create_query = (LinkCreateQuery) {
                        .first = node_vector_get(rand() % node_vector_get_size(node_vector), node_vector),
                        .link = &(Link) {
                                .type = rand() % 3,
                                .name = &link_names[rand() % 2]
                        },
                        .second = node_vector_get(rand() % node_vector_get_size(node_vector), node_vector),
                }
        };
        query_execute(GET_ALL_LINKS, connection);
    }

    printf("\nAll links:\n");
    Query GET_ALL_LINKS = (Query) {
            .type = MATCH,
            .match_query = (MatchQuery) {
                    .return_items = RETURN_ALL,
                    .condition = NO_CONDITION
            }
    };
    qr = query_execute(GET_ALL_LINKS, connection);
    ResultVector *result_vector = result_vector_fetch_all(qr.generator);
    V_ITERATE(result_vector, i) {
        Result *res = result_vector_get(i, result_vector);
        result_print(res);
        printf("\n");
    }

    printf("\n");

    printf("\nAll nodes:\n");
    Query MATCH_GET_ALL_NODES = (Query) {
            .type = MATCH,
            .match_query = (MatchQuery) {
                    .return_items = RETURN_NODE_FIRST,
                    .condition = &(MatchCondition) {
                            .node_first_condition = &(NodeCondition) {
                                    .schema = schema,
                            },
                    }
            }
    };
    qr = query_execute(MATCH_GET_ALL_NODES, connection);
    result_vector = result_vector_fetch_all(qr.generator);
    V_ITERATE(result_vector, i) {
        Result *res = result_vector_get(i, result_vector);
        result_print(res);
        printf("\n");
    }
    printf("\n");

    Query GET_LINKS_TO_UPDATE = (Query) {
            .type = MATCH,
            .match_query = (MatchQuery) {
                    .return_items = RETURN_ALL,
                    .condition = (MatchCondition) {
                            .node_first_condition = (NodeCondition) {
                                    .schema = schema,
                                    .conditions_num = 1,
                                    .compare_conditions = (ConditionCompare[1]) {
                                            {
                                                    {.type = CMPT_FIELD_NAME, .field_name = "male"},
                                                    CMP_EQUAL,
                                                    {.type = CMPT_VALUE, .value = &(Value) {.data_type = DT_BOOLEAN,
                                                            true}}
                                            }
                                    }
                            },
                            .link_condition = (LinkCondition) {
                                    .link_name = &(String) {5, "HATES"},
                                    .link_type = LT_DIRECTED_TO
                            }
                    }
            }
    };
    qr = query_execute(GET_LINKS_TO_UPDATE, connection);
    LinkVector *link_vector = link_vector_init();
    Result *res;
    Link *link_to_add;
    printf("Links to update:\n");
    while ((res = generator_next(qr.generator)) != NULL) {
        link_to_add = link_copy(res->link);
        link_vector_push(link_to_add, link_vector);
        result_print(res);
        printf("\n");
    }

    Query UPDATE_LINK = (Query) {
            .type = LINK_UPDATE,
            .link_update_query = (LinkUpdateQuery) {
                    .link_vector = link_vector,
                    .new_name = (String) {8, "Zeleboba"},
                    .new_type = LT_DIRECTED_BOTH
            },
    };
    qr = query_execute(UPDATE_LINK, connection);

    link_vector_free(link_vector);
    link_vector = link_vector_init();
    qr = query_execute(GET_ALL_LINKS, connection);
    printf("Links after update:\n");
    while ((res = generator_next(qr.generator)) != NULL) {
        link_to_add = link_copy(res->link);
        link_vector_push(link_to_add, link_vector);
        result_print(res);
        printf("\n");
    }

    Query GET_ZELEBOBA_LINKS = (Query) {
            .type = MATCH,
            .match_query = (MatchQuery) {
                    .return_items = RETURN_ALL,
                    .condition = (MatchCondition) {
                            .link_condition = (LinkCondition) {
                                    .link_name = &(String) {8, "Zeleboba"},
                                    .link_type = LT_ANY,
                            }
                    }
            }
    };

    printf("Links to delete:\n");
    link_vector_free(link_vector);
    link_vector = link_vector_init();
    qr = query_execute(GET_ZELEBOBA_LINKS, connection);
    while ((res = generator_next(qr.generator)) != NULL) {
        link_to_add = link_copy(res->link);
        link_vector_push(link_to_add, link_vector);
        result_print(res);
        printf("\n");
    }

    Query DELETE_LINKS = (Query) {
            .type = LINK_DELETE,
            .link_delete_query = (LinkDeleteQuery) {
                    .link_vector = link_vector
            }
    };

    qr = query_execute(DELETE_LINKS, connection);

    printf("All links after delete:\n");
    qr = query_execute(GET_ALL_LINKS, connection);
    result_vector = result_vector_fetch_all(qr.generator);
    V_ITERATE(result_vector, i) {
        res = result_vector_get(i, result_vector);
        result_print(res);
        printf("\n");
    }
}


#define STRESS_NODES 1000

#define COUPLES 10

void fill_nodes(Schema *schema, Connection *connection) {
    for (size_t i = 0; i < STRESS_NODES; i++)
        insert_random_node(schema, connection);
}

NodeCondition get_node_condition(Schema *schema) {
    return (NodeCondition){
            .schema = schema,
            .conditions_num = 2,
            .compare_conditions = (ConditionCompare[2]) {
                    {
                            .left = {.type = CMPT_FIELD_NAME, .field_name = "id"},
                            .cmp = CMP_GREATER,
                            .right = {.type = CMPT_VALUE, .value = &(Value) {
                                    .data_type = DT_INTEGER, .integer = rand()
                            }}
                    },
                    {
                            .left = {.type = CMPT_FIELD_NAME, .field_name = "name"},
                            .cmp = CMP_LESS,
                            .right = {.type = CMPT_VALUE, .value = &(Value) {
                                    .data_type = DT_STRING, .string = &(String) {7, "naaaaaa"}
                            }}
                    }
            }};
}

void test_stress(Connection *connection) {
    Query GET_SCHEMA = (Query) {.type = SCHEMA_GET, .schema_get_query = (SchemaGetQuery) {.schema_name = TEST_SCHEMA_NAME}};
    QueryResult qr = query_execute(GET_SCHEMA, connection);
    Schema *schema = qr.schema;
    schema_print(schema);

    printf("Creation nodes\n");
    for (size_t _ = 0; _ < COUPLES; _++) {
        clock_t tic = clock();
        fill_nodes(schema, connection);
        clock_t toc = clock();
        double spent = (double) (toc - tic) / CLOCKS_PER_SEC;
        printf("%f ", spent);
    }
    printf("\n");

    NodeVector *node_vector;

    printf("Getting nodes\n");
    for (size_t _ = 0; _ < COUPLES; _++) {
        clock_t tic = clock();

        fill_nodes(schema, connection);

        qr = query_execute((Query) {.type = NODE_MATCH, .node_match_query = get_node_condition(schema)}, connection);
        node_vector = node_vector_fetch_all(qr.generator);
        node_vector_free(node_vector);

        clock_t toc = clock();
        double spent = (double) (toc - tic) / CLOCKS_PER_SEC;
        printf("%f ", spent);
    }
    printf("\n");

    printf("Updating nodes\n");
    for (size_t _ = 0; _ < COUPLES; _++) {
        clock_t tic = clock();
        fill_nodes(schema, connection);
        qr = query_execute((Query) {.type = NODE_MATCH, .node_match_query = get_node_condition(schema)}, connection);
        node_vector = node_vector_fetch_all(qr.generator);
        Query query = (Query) {
            .type = NODE_UPDATE,
            .node_update_query = (NodeUpdateQuery) {
                .schema = schema,
                .node_vector = node_vector,
                .attributes_to_update_number = 0,
                .attributes_to_update = (char*[]){},
                .new_values = (Value*[]){}
            }
        };
        query_execute(query, connection);
        node_vector_free(node_vector);
        clock_t toc = clock();
        double spent = (double) (toc - tic) / CLOCKS_PER_SEC;
        printf("%f ", spent);
    }
    printf("\n");

    printf("Deleting nodes\n");
    for (size_t _ = 0; _ < COUPLES; _++) {
        clock_t tic = clock();
        fill_nodes(schema, connection);
        qr = query_execute((Query) {.type = NODE_MATCH, .node_match_query = get_node_condition(schema)}, connection);
        node_vector = node_vector_fetch_all(qr.generator);
        Query query = (Query) {
                .type = NODE_DELETE,
                .node_delete_query = (NodeDeleteQuery) {
                        .schema = schema,
                        .node_vector = node_vector
                }
        };
        query_execute(query, connection);
        node_vector_free(node_vector);
        clock_t toc = clock();
        double spent = (double) (toc - tic) / CLOCKS_PER_SEC;
        printf("%f ", spent);
    }
    printf("\n");
}

RC test(Connection *connection) {
    RC rc = RC_OK;

    test_prepare(connection);
    test_manipulate(connection);
    test_stress(connection);

    return rc;
}

#endif //LOW_LEVEL_PROGRAMMING_TASK1_TEST_H
