#ifndef LOW_LEVEL_PROGRAMMING_TASK1_BLOCK_NODE_H
#define LOW_LEVEL_PROGRAMMING_TASK1_BLOCK_NODE_H

#include "../model/block_model.h"

typedef struct BlockNode {
    pointer block_pointer;
    Block *block;
    struct BlockNode *next;
} BlockNode;

BlockNode *block_node_init(Block *block);
void block_node_free(BlockNode *block_node);
BlockNode *block_node_copy(BlockNode *src);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_BLOCK_NODE_H
