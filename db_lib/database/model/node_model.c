#include <malloc.h>
#include <string.h>
#include "node_model.h"


DB_Node *db_node_init() {
    DB_Node *db_node = malloc(sizeof(DB_Node));
    db_node->value_pointers = NULL;
    return db_node;
}

amount db_node_size(DB_Node *db_node) {
    return (
            sizeof(db_node->schema_pointer) +
            sizeof(db_node->value_count) +
            db_node->value_count * sizeof(*db_node->value_pointers) +
            sizeof(db_node->node_prev_pointer) +
            sizeof(db_node->node_next_pointer)
    );
}

void db_node_free(DB_Node *db_node) {
    free(db_node->value_pointers);
    free(db_node);
}

Node *node_init() {
    Node *node = malloc(sizeof(Node));
    return node;
}

amount node_size(Node *node) {
    amount values_size = 0;
    for (amount i = 0; i < node->value_count; i++)
        values_size += value_size(node->values[i]);
    return (
            sizeof(node->value_count) +
            values_size
    );
}

void node_free(Node *node) {
    if (node != NULL)
        for (amount i = 0; i < node->value_count; i++)
            value_free(node->values[i]);
    free(node);
}

Node *node_copy(Node *src) {
    Node *node = node_init();
    memcpy(node, src, sizeof(Node));
    node->values = malloc(src->value_count * sizeof(*node->values));
    for (size_t i = 0; i < src->value_count; i++)
        node->values[i] = value_copy(src->values[i]);
    return node;
}