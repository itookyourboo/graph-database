#ifndef LOW_LEVEL_PROGRAMMING_TASK1_NODE_CRUD_H
#define LOW_LEVEL_PROGRAMMING_TASK1_NODE_CRUD_H

#include "crud_util.h"
#include "../condition.h"
#include "../data_structure/generator.h"
#include "../model/result_model.h"
#include "../data_structure/vector.h"

DECLARE_INSERT_FUNCTION(DB_Node, db_node)

Generator *match_nodes(
        NodeCondition condition,
        Connection *connection
);

RC create_node(
        Schema *schema,
        Node *node,
        Connection *connection
);

RC update_nodes(
        Schema *schema,
        NodeVector *node_vector,
        amount attributes_to_update_number,
        char **attributes_to_update,
        Value **new_values,
        Connection *connection
);

RC delete_nodes(
        NodeVector *node_vector,
        Connection *connection
);


bool check_node_condition(
        DB_Node *db_node,
        NodeCondition condition,
        Connection *connection
);

bool check_cross_node_condition(
        Schema *schema_first,
        DB_Node *db_node_first,
        Schema *schema_second,
        DB_Node *db_node_second,
        NodeCondition condition,
        Connection *connection
);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_NODE_CRUD_H
