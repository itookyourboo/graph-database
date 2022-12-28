#ifndef LOW_LEVEL_PROGRAMMING_TASK1_VISUALIZE_H
#define LOW_LEVEL_PROGRAMMING_TASK1_VISUALIZE_H

#include "mem.h"
#include "model/string_model.h"
#include "data_structure/block_heap.h"
#include "model/schema_model.h"
#include "model/node_model.h"
#include "model/link_model.h"
#include "model/result_model.h"

void string_print(String *string);
void block_node_print(BlockNode *block_node);
void block_heap_print(BlockHeap *block_heap);
void schema_print(Schema *schema);
void link_print(Link *link);
void node_print(Node *node);
void value_print(Value *value);
void result_print(Result *result);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_VISUALIZE_H
