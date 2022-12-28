#ifndef LOW_LEVEL_PROGRAMMING_TASK1_NODE_MODEL_H
#define LOW_LEVEL_PROGRAMMING_TASK1_NODE_MODEL_H

#include "../mem.h"
#include "value_model.h"

typedef struct __attribute__((packed)) {
    pointer schema_pointer;
    amount value_count;
    pointer *value_pointers;
    pointer node_prev_pointer;
    pointer node_next_pointer;
} DB_Node;

DB_Node *db_node_init();
amount db_node_size(DB_Node *db_node);
void db_node_free(DB_Node *db_node);

typedef struct {
    amount value_count;
    Value **values;

    pointer _db_ptr;
    pointer _node_prev_ptr;
    pointer _node_next_ptr;

    pointer _schema_ptr;
} Node;

Node *node_init();
amount node_size(Node *node);
void node_free(Node *node);
Node *node_copy(Node *src);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_NODE_MODEL_H
