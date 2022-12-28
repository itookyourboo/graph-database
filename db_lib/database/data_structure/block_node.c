#include <malloc.h>
#include "block_node.h"

BlockNode *block_node_init(Block *block) {
    BlockNode *block_node = malloc(sizeof(BlockNode));
    block_node->block_pointer = nullptr;
    block_node->block = block;
    block_node->next = NULL;
    return block_node;
}

void block_node_free(BlockNode *block_node) {
    block_free(block_node->block);
    free(block_node);
}

// stub for using vector.h
BlockNode *block_node_copy(BlockNode *src) {
    return src;
}