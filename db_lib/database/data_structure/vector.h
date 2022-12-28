#ifndef LOW_LEVEL_PROGRAMMING_TASK1_VECTOR_H
#define LOW_LEVEL_PROGRAMMING_TASK1_VECTOR_H

#include "vector_abstract.h"
#include "../model/result_model.h"
#include "../model/node_model.h"
#include "../model/link_model.h"
#include "block_node.h"

DECLARE_VECTOR(BlockNode, block_node)
DECLARE_VECTOR(Result, result)
DECLARE_VECTOR(Node, node)
DECLARE_VECTOR(Link, link)
DECLARE_VECTOR(Schema, schema)

#define V_ITERATE(vector, index) \
for (size_t (index) = 0; (index) < vector##_get_size(vector); (index)++)

#define V_GET(vector, index) \
vector##_get(index, vector)

#define V_FOREACH(vector, item) \
for (size_t index = 0; index < vector##_get_size(vector); (item) = V_GET(vector, index++))

#endif //LOW_LEVEL_PROGRAMMING_TASK1_VECTOR_H
