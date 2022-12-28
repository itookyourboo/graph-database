#include "../connection.h"
#include "node_serialize.h"
#include "util.h"
#include "block_serialize.h"
#include "value_serialize.h"


RC db_node_to_bytes(DB_Node *db_node, byte *bytes) {
    amount i = 0;
    field_to_bytes(db_node, schema_pointer, bytes, i);
    field_to_bytes(db_node, value_count, bytes, i);
    field_array_to_bytes(db_node, value_pointers, db_node->value_count, bytes, i);
    field_to_bytes(db_node, node_prev_pointer, bytes, i);
    field_to_bytes(db_node, node_next_pointer, bytes, i);
    return RC_OK;
}

RC db_node_from_bytes(DB_Node *db_node, byte *bytes) {
    amount i = 0;
    field_from_bytes(db_node, schema_pointer, bytes, i);
    field_from_bytes(db_node, value_count, bytes, i);
    field_array_from_bytes(db_node, value_pointers, db_node->value_count, bytes, i);
    field_from_bytes(db_node, node_prev_pointer, bytes, i);
    field_from_bytes(db_node, node_next_pointer, bytes, i);
    return RC_OK;
}

RC db_node_to_node(DB_Node *db_node, Node *node, Connection *connection) {
    RC rc = RC_OK;
    node->_schema_ptr = db_node->schema_pointer;
    node->_node_next_ptr = db_node->node_next_pointer;
    node->_node_prev_ptr = db_node->node_prev_pointer;
    node->value_count = db_node->value_count;
    node->values = malloc(node->value_count * sizeof(*node->values));

    Block *block_value = block_init();
    DB_Value *db_value;

    for (amount i = 0; i < node->value_count; i++) {
        rc = connection_move_to_pointer(db_node->value_pointers[i], connection);
        throw_if_not_ok(rc);
        rc = block_from_file(block_value, connection);
        throw_if_not_ok(rc);
        node->values[i] = value_init();

        db_value = db_value_init();
        rc = db_value_from_bytes(db_value, block_value->content);
        db_value_to_value(db_value, node->values[i], connection);
        throw_if_not_ok(rc);
    }
    return rc;
}

RC db_node_from_node(DB_Node *db_node, Node *node) {
    db_node->schema_pointer = node->_schema_ptr;
    db_node->value_count = node->value_count;
    db_node->value_pointers = malloc(db_node->value_count * sizeof(*db_node->value_pointers));
    db_node->node_prev_pointer = node->_node_prev_ptr;
    db_node->node_next_pointer = node->_node_next_ptr;
    return RC_OK;
}
