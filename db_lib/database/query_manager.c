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

QueryResult execute_node_create(Query query, Connection *connection) {
    NodeCreateQuery q = query.node_create_query;
    RC rc = create_node(q.schema, q.node, connection);
    return (QueryResult) {
            .type = RESULT_NONE,
            .status = rc
    };
}

QueryResult execute_node_update(Query query, Connection *connection) {
    NodeUpdateQuery q = query.node_update_query;
    RC rc = update_nodes(
            q.schema,
            q.node_vector,
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
    RC rc = delete_nodes(q.node_vector, connection);
    return (QueryResult) {
        .type = RESULT_NONE,
        .status = rc
    };
}

QueryResult execute_node_match(Query query, Connection *connection) {
    NodeMatchQuery q = query.node_match_query;
    Generator *generator = match_nodes(q.condition, connection);
    return (QueryResult) {
        .type = RESULT_GENERATOR,
        .generator = generator
    };
}

QueryResult execute_link_create(Query query, Connection *connection) {
    LinkCreateQuery q = query.link_create_query;
    RC rc = link_create(q.link, q.first, q.second, connection);
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
    RC rc = link_delete(q.link_vector, connection);
    return (QueryResult) {
        .type = RESULT_NONE,
        .status = rc
    };
}

QueryResult execute_match(Query query, Connection *connection) {
    MatchQuery q = query.match_query;
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
