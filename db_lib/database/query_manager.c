#include "query_manager.h"
#include "data_manipulation/cruds.h"

QueryResult execute_schema_create(Query query, Connection *connection) {
    SchemaCreateQuery q = query.schema_create_query;
    RC rc = schema_create(q.schema, connection);
    return (QueryResult) {
            .type = RESULT_NONE,
    };
}

QueryResult execute_schema_get(Query query, Connection *connection) {
    Schema *schema = schema_init();
    SchemaGetQuery q = query.schema_get_query;
    schema_show(q.schema_name, schema, connection);
    return (QueryResult) {
            .type = RESULT_SCHEMA,
            .schema = schema
    };
}

QueryResult execute_schema_get_all(Query query, Connection *connection) {
    Generator *generator = all_schemas(connection);
    return (QueryResult) {
            .type = RESULT_GENERATOR,
            .generator = generator
    };
}

QueryResult execute_schema_delete(Query query, Connection *connection) {
    SchemaDeleteQuery q = query.schema_delete_query;
    RC rc = schema_delete(q.schema_name, connection);
    return (QueryResult) {
            .type = RESULT_NONE,
            .status = rc
    };
}

void update_node_condition(NodeCondition *node_condition, Connection *connection) {
    if (node_condition->schema == NULL && node_condition->schema_name != NULL) {
        node_condition->schema = schema_init();
        schema_show(node_condition->schema_name, node_condition->schema, connection);
    }
}

QueryResult execute_node_create(Query query, Connection *connection) {
    NodeCreateQuery q = query.node_create_query;
    if (!q.schema) {
        q.schema = schema_init();
        schema_show(q.schema_name, q.schema, connection);
    }
    RC rc = create_node(q.schema, q.node, connection);
    return (QueryResult) {
            .type = RESULT_NONE,
            .status = rc
    };
}

QueryResult execute_node_match(Query query, Connection *connection) {
    NodeMatchQuery q = query.node_match_query;
    update_node_condition(&q.condition, connection);
    Generator *generator = match_nodes(q.condition, connection);
    return (QueryResult) {
            .type = RESULT_GENERATOR,
            .generator = generator
    };
}

QueryResult execute_node_update(Query query, Connection *connection) {
    NodeUpdateQuery q = query.node_update_query;
    update_node_condition(&q.condition, connection);

    Generator *generator = match_nodes(q.condition, connection);
    NodeVector *node_vector = node_vector_fetch_all(generator);

    RC rc = update_nodes(
            q.condition.schema,
            node_vector,
            q.attributes_to_update_number,
            q.attributes_to_update,
            q.new_values,
            connection
    );

    return (QueryResult) {
            .type = RESULT_NONE,
            .status = rc
    };
}

QueryResult execute_node_delete(Query query, Connection *connection) {
    NodeDeleteQuery q = query.node_delete_query;
    update_node_condition(&q.condition, connection);

    Generator *generator = match_nodes(q.condition, connection);
    NodeVector *node_vector = node_vector_fetch_all(generator);

    RC rc = delete_nodes(node_vector, connection);
    return (QueryResult) {
        .type = RESULT_NONE,
        .status = rc
    };
}

QueryResult execute_link_create(Query query, Connection *connection) {
    LinkCreateQuery q = query.link_create_query;

    update_node_condition(&q.first, connection);
    Generator *generator = match_nodes(q.first, connection);
    NodeVector *nodes_first = node_vector_fetch_all(generator);

    update_node_condition(&q.second, connection);
    generator = match_nodes(q.first, connection);
    NodeVector *nodes_second = node_vector_fetch_all(generator);

    RC rc = RC_OK;

    for (size_t i = 0; i < node_vector_get_size(nodes_first); i++) {
        for (size_t j = 0; j < node_vector_get_size(nodes_second); j++) {
            rc = link_create(
                    q.link,
                    node_vector_get(i, nodes_first),
                    node_vector_get(j, nodes_second),
                    connection);
            if (rc != RC_OK)
                return (QueryResult) {
                        .type = RESULT_NONE,
                        .status = rc
                };
        }
    }

    return (QueryResult) {
        .type = RESULT_NONE,
        .status = rc
    };
}

QueryResult execute_link_update(Query query, Connection *connection) {
    LinkUpdateQuery q = query.link_update_query;
    RC rc = link_update(q.link_vector, q.new_name, q.new_type, connection);
    return (QueryResult) {
        .type = RESULT_NONE,
        .status = rc,
    };
}

QueryResult execute_link_delete(Query query, Connection *connection) {
    LinkDeleteQuery q = query.link_delete_query;

    Generator *generator = match(q.condition, RETURN_LINK, connection);
    ResultVector *result_vector = result_vector_fetch_all(generator);

    LinkVector *link_vector = link_vector_init();
    for (size_t i = 0; i < result_vector_get_size(result_vector); i++) {
        Link *link = link_copy(result_vector_get(i, result_vector)->link);
        link_vector_push(link, link_vector);
    }

    RC rc = link_delete(link_vector, connection);
    return (QueryResult) {
        .type = RESULT_NONE,
        .status = rc
    };
}

QueryResult execute_match(Query query, Connection *connection) {
    MatchQuery q = query.match_query;
    update_node_condition(&q.condition.node_first_condition, connection);
    update_node_condition(&q.condition.node_second_condition, connection);
    update_node_condition(&q.condition.node_cross_condition, connection);
    Generator *generator = match(q.condition, q.return_items, connection);
    return (QueryResult) {
        .type = RESULT_GENERATOR,
        .generator = generator
    };
}


#define EXECUTORS_NUM (MATCH + 1)


QueryResult (*executors[EXECUTORS_NUM])(Query, Connection*) = {
        &execute_schema_create,
        &execute_schema_get,
        &execute_schema_get_all,
        &execute_schema_delete,

        &execute_node_create,
        &execute_node_update,
        &execute_node_delete,
        &execute_node_match,

        &execute_link_create,
        &execute_link_update,
        &execute_link_delete,

        &execute_match
};

QueryResult query_execute(Query query, Connection *connection) {
    return executors[query.type](query, connection);
}
