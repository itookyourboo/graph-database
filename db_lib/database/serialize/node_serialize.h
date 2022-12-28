#ifndef LOW_LEVEL_PROGRAMMING_TASK1_NODE_SERIALIZE_H
#define LOW_LEVEL_PROGRAMMING_TASK1_NODE_SERIALIZE_H

#include "../exceptions.h"
#include "../model/node_model.h"
#include "../connection.h"

RC db_node_to_bytes(DB_Node *db_node, byte *bytes);
RC db_node_from_bytes(DB_Node *db_node, byte *bytes);
RC db_node_to_node(DB_Node *db_node, Node *node, Connection *connection);
RC db_node_from_node(DB_Node *db_node, Node *node);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_NODE_SERIALIZE_H
