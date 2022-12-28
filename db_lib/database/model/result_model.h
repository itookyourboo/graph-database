#ifndef LOW_LEVEL_PROGRAMMING_TASK1_RESULT_MODEL_H
#define LOW_LEVEL_PROGRAMMING_TASK1_RESULT_MODEL_H

#include "node_model.h"
#include "link_model.h"
#include "schema_model.h"
#include "../condition.h"

typedef enum {
    RETURN_NOTHING      = 0b000,
    RETURN_NODE_FIRST   = 0b001,
    RETURN_NODE_SECOND  = 0b010,
    RETURN_LINK         = 0b100,
    RETURN_ALL          = 0b111,

    RETURN_UNDEF        = 0b1000,
} Return;

size_t return_count(Return return_items);

typedef struct {
    Return return_items;
    Node *first;
    Link *link;
    Node *second;
} Result;

Result *result_init();
void result_free(Result *result);
Result *result_copy(Result *src);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_RESULT_MODEL_H
