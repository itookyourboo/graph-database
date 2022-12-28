#include "node_crud.h"
#include "schema_crud.h"
#include "value_crud.h"

DEFINE_INSERT_FUNCTION(DB_Node, db_node)


#define GET_VALUE(db_node, schema, side)                                                            \
do {                                                                                                \
    if (ct_cmp.side.type == CMPT_VALUE) {                                                           \
        (side) = ct_cmp.side.value;                                                                 \
    } else if (ct_cmp.side.type == CMPT_FIELD_NAME) {                                               \
        (side) = value_init();                                                                      \
        amount attr_index = get_attribute_index((schema), ct_cmp.side.field_name);                  \
        connection_move_to_pointer((db_node)->value_pointers[attr_index], connection);              \
        RC rc = value_retrieve(side, connection);                                                   \
        throw_if_not_ok(rc);                                                                        \
    }                                                                                               \
} while(0)                                                                                          \


amount get_attribute_index(Schema *schema, char *field_name) {
    amount i;
    for (i = 0; i < schema->attr_count; i++)
        if (string_char_cmp(schema->attributes[i]->name, field_name) == 0)
            break;
    return i;
}


bool check_node_condition(
        DB_Node *db_node,
        NodeCondition condition,
        Connection *connection
) {
    if (condition.is_null)
        return true;

    Schema *schema = condition.schema;
    if (schema == NULL)
        return true;

    if (db_node->schema_pointer != schema->_db_ptr)
        return false;

    Value *left = NULL, *right = NULL;
    for (amount i = 0; i < condition.conditions_num; i++) {
        ConditionCompare ct_cmp = condition.compare_conditions[i];
        GET_VALUE(db_node, schema, left);
        GET_VALUE(db_node, schema, right);
        if (value_compare(left, right) != ct_cmp.cmp) {
            return false;
        }
    }

    return true;
}

bool check_cross_node_condition(
        Schema *schema_first,
        DB_Node *db_node_first,
        Schema *schema_second,
        DB_Node *db_node_second,
        NodeCondition condition,
        Connection *connection
) {
    Block *block = block_init();

    if (condition.is_null)
        return true;

    for (amount i = 0; i < condition.conditions_num; i++) {
        ConditionCompare ct_cmp = condition.compare_conditions[i];
        Value *left, *right;
        GET_VALUE(db_node_first, schema_first, left);
        GET_VALUE(db_node_second, schema_second, right);
        if (value_compare(left, right) != ct_cmp.cmp) {
            return false;
        }
    }

    block_free(block);
    return true;
}


RC create_node(
        Schema *schema,
        Node *node,
        Connection *connection
) {
    RC rc = RC_OK;

    DB_Header *db_header = connection->buffer_manager->db_header;

    DB_Node *db_node = &(DB_Node) {
            .schema_pointer = schema->_db_ptr,
            .value_count = node->value_count,
            .node_prev_pointer = db_header->node_last,
            .node_next_pointer = nullptr,
    };
    db_node->value_pointers = malloc(db_node->value_count * sizeof(*db_node->value_pointers));

    for (amount i = 0; i < db_node->value_count; i++) {
        rc = value_insert(node->values[i], &db_node->value_pointers[i], connection);
        throw_if_not_ok(rc);
    }

    pointer node_pointer;
    rc = db_node_insert(db_node, &node_pointer, connection);
    throw_if_not_ok(rc);

    if (db_header->node_first == nullptr)
        db_header->node_first = node_pointer;
    else {
        rc = change_next_pointer(
                db_header->node_last,
                node_pointer,
                connection
        );
        throw_if_not_ok(rc);
    }
    db_header->node_last = node_pointer;

    rc = database_update_header(connection);
    throw_if_not_ok(rc);

    free(db_node->value_pointers);
    return rc;
}

#define RETRIEVE_DB_NODE()                                                                                          \
connection_move_to_pointer(current_node_pointer, connection);                                                       \
block_from_file(block, connection);                                                                                 \
db_node_from_bytes(db_node, block->content)                                                                         \


#define INIT_POINTERS() \
DB_Node *db_node = db_node_init();                                                          \
Block *block = block_init();                                                                \
Value *value = value_init()


#define FREE_POINTERS()                                                                                             \
block_free(block);                                                                                                  \
value_free(value)

typedef struct {
    pointer prev;
    pointer cur;
    Schema *schema;
    NodeCondition condition;
    Connection *connection;
} ResultContext;

Node *node_next(Generator *generator) {
    ResultContext *context = generator->context;
    Node *node = generator->result;

    pointer current_node_pointer;
    Connection *connection = context->connection;

    RC rc;
    INIT_POINTERS();

    while (context->cur != nullptr) {
        current_node_pointer = context->cur;
        RETRIEVE_DB_NODE();

        bool matched = check_node_condition(db_node, context->condition, connection);
        if (matched) {
            rc = db_node_to_node(db_node, node, connection);
            node->_db_ptr = context->cur;
            throw_if_not_ok(rc);
        }

        context->prev = current_node_pointer;
        context->cur = db_node->node_next_pointer;

        if (matched) {
            return generator->result;
        }
    }

    FREE_POINTERS();
    return (generator->result = NULL);
}


Generator *match_nodes(
        NodeCondition condition,
        Connection *connection
) {
    Generator *generator = generator_init();
    generator->result = node_init();

    ResultContext *context = malloc(sizeof(ResultContext));
    generator->context = context;

    context->cur = connection->buffer_manager->db_header->node_first;
    context->prev = nullptr;
    context->condition = condition;
    context->schema = condition.is_null ? NULL : condition.schema;
    context->connection = connection;

    generator->next_function = (void *(*)(Generator *)) node_next;

    return generator;
}

RC update_nodes(
        Schema *schema,
        NodeVector *node_vector,
        amount attributes_to_update_number,
        char **attributes_to_update,
        Value **new_values,
        Connection *connection
) {
    RC rc = RC_OK;
    INIT_POINTERS();
    pointer current_node_pointer = nullptr;

    Node *node;
    for (size_t i = 0; i < node_vector_get_size(node_vector); i++) {
        node = node_vector_get(i, node_vector);
        current_node_pointer = node->_db_ptr;
        RETRIEVE_DB_NODE();
        for (amount j = 0; j < attributes_to_update_number; j++) {
            amount index = get_attribute_index(schema, attributes_to_update[j]);
            connection_move_to_pointer(db_node->value_pointers[index], connection);
            value_update(new_values[j], connection);
        }
    }

    FREE_POINTERS();
    return rc;
}

RC delete_nodes(
        NodeVector *node_vector,
        Connection *connection
) {
    RC rc = RC_OK;
    INIT_POINTERS();

    DB_Header *db_header = connection->buffer_manager->db_header;

    Node *node;
    for (size_t ind = 0; ind < node_vector_get_size(node_vector); ind++) {
        node = node_vector_get(ind, node_vector);
        pointer current_node_pointer = node->_db_ptr;
        RETRIEVE_DB_NODE();

        amount block_num = 2 * db_node->value_count + 1;
        pointer block_pointers_to_free[block_num];
        amount i, j;

        DB_Value *db_value = db_value_init();
        for (i = 0, j = 0; j < db_node->value_count; j++) {
            connection_move_to_pointer(db_node->value_pointers[j], connection);
            db_value_retrieve(db_value, connection);
            block_pointers_to_free[i++] = db_node->value_pointers[j];
            if (db_value->data_type == DT_STRING)
                block_pointers_to_free[i++] = db_value->string_pointer;
        }
        block_pointers_to_free[i++] = node->_db_ptr;
        database_free_blocks(block_pointers_to_free, i, connection);

        if (node->_db_ptr == db_header->node_first)
            db_header->node_first = db_node->node_next_pointer;

        if (db_node->node_next_pointer != nullptr)
            change_prev_pointer(db_node->node_next_pointer, db_node->node_prev_pointer, connection);
        if (db_node->node_prev_pointer != nullptr)
            change_next_pointer(db_node->node_prev_pointer, db_node->node_next_pointer, connection);

        if (node->_db_ptr == db_header->node_last)
            db_header->node_last = db_node->node_prev_pointer;
    }

    database_update_header(connection);

    FREE_POINTERS();
    return rc;
}
