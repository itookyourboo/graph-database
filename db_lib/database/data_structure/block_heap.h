#ifndef LOW_LEVEL_PROGRAMMING_TASK1_BLOCK_HEAP_H
#define LOW_LEVEL_PROGRAMMING_TASK1_BLOCK_HEAP_H

#include <stdbool.h>
#include "../mem.h"
#include "block_node.h"
#include "vector.h"

typedef struct {
    BlockNodeVector *block_node_vector;
} BlockHeap;

BlockHeap *block_heap_init(BlockNodeVector *block_node_vector);
BlockHeap *block_heap_free(BlockHeap *block_heap);

amount block_heap_size(BlockHeap *block_heap);
amount block_heap_capacity(BlockHeap *block_heap);
void block_heap_insert(BlockNode *block_node, BlockHeap *block_heap);
void block_heap_push(BlockNode *block_node, BlockHeap *block_heap);
void block_heap_build(BlockHeap *block_heap);
void block_heap_sift_down(amount index, BlockHeap *block_heap);
void block_heap_sift_up(amount index, BlockHeap *block_heap);
BlockNode *block_heap_extract(BlockHeap *block_heap);
BlockNode *block_heap_peek(BlockHeap *block_heap);
bool block_heap_can_occupy_max(amount size_to_occupy, BlockHeap *block_heap);
BlockNode *block_heap_occupy_max(amount size_to_occupy, BlockHeap *block_heap);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_BLOCK_HEAP_H
