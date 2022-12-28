#include <malloc.h>
#include "block_heap.h"

BlockHeap *block_heap_init(BlockNodeVector *block_node_vector) {
    BlockHeap *block_heap = malloc(sizeof(BlockHeap));
    if (block_node_vector == NULL)
        block_node_vector = block_node_vector_init();
    block_heap->block_node_vector = block_node_vector;
    return block_heap;
}

BlockHeap *block_heap_free(BlockHeap *block_heap) {
    block_node_vector_free(block_heap->block_node_vector);
    free(block_heap);
}

amount block_heap_size(BlockHeap *block_heap) {
    return block_node_vector_get_size(block_heap->block_node_vector);
}

amount block_heap_capacity(BlockHeap *block_heap) {
    return block_node_vector_get_capacity(block_heap->block_node_vector);
}

int cmp_nodes(BlockNode *block_node1, BlockNode *block_node2) {
    amount size1 = block_node1->block->size;
    amount size2 = block_node2->block->size;
    return (size1 > size2) - (size1 < size2);
}

void swap_nodes(BlockNode *block_node1, BlockNode *block_node2) {
    BlockNode temp = *block_node1;
    *block_node1 = *block_node2;
    *block_node2 = temp;
}

BlockNode *get(amount index, BlockHeap *block_heap) {
    return block_node_vector_get(index, block_heap->block_node_vector);
}

void set(amount index, BlockNode *block_node, BlockHeap *block_heap) {
    block_node_vector_set(index, block_node, block_heap->block_node_vector);
}

void swap_indexes(amount a, amount b, BlockHeap *block_heap) {
    BlockNode *a_node = get(a, block_heap);
    BlockNode *b_node = get(b, block_heap);
    swap_nodes(a_node, b_node);
}

int cmp_indexes(amount a, amount b, BlockHeap *block_heap) {
    BlockNode *a_node = get(a, block_heap);
    BlockNode *b_node = get(b, block_heap);
    return cmp_nodes(a_node, b_node);
}

void block_heap_push(BlockNode *block_node, BlockHeap *block_heap) {
    block_node_vector_push(block_node, block_heap->block_node_vector);
}

void block_heap_insert(BlockNode *block_node, BlockHeap *block_heap) {
    block_heap_push(block_node, block_heap);
    block_heap_sift_up(block_heap_size(block_heap) - 1, block_heap);
}

void block_heap_build(BlockHeap *block_heap) {
    amount i = block_heap_size(block_heap) / 2;
    if (i == 0) return;
    do block_heap_sift_down(--i, block_heap); while (i > 0);
}

void block_heap_sift_down(amount i, BlockHeap *block_heap) {
    amount greatest, left, right;
    amount size = block_heap_size(block_heap);

    while (2 * i + 1 < size) {
        left = 2 * i + 1;
        right = 2 * i + 2;
        greatest = left;
        if (right < size && cmp_indexes(right, left, block_heap) > 0)
            greatest = right;
        if (cmp_indexes(i, greatest, block_heap) >= 0)
            break;
        swap_indexes(i, greatest, block_heap);
        i = greatest;
    }
}

void block_heap_sift_up(amount index, BlockHeap *block_heap) {
    while (index != 0 && cmp_indexes(index, (index - 1) / 2, block_heap) > 0) {
        swap_indexes(index, (index - 1) / 2, block_heap);
        index = (index - 1) / 2;
    }
}

BlockNode *block_heap_extract(BlockHeap *block_heap) {
    amount last_index = block_heap_size(block_heap) - 1;
    BlockNode *min = get(0, block_heap);
    set(0, get(last_index, block_heap), block_heap);
    block_heap_sift_down(0, block_heap);
    return min;
}

BlockNode *block_heap_peek(BlockHeap *block_heap) {
    return get(0, block_heap);
}

bool block_heap_can_occupy_max(amount size_to_occupy, BlockHeap *block_heap) {
    if (block_heap_size(block_heap) == 0) return false;
    BlockNode *max_node = block_heap_peek(block_heap);
    if (!max_node->block->is_free) return false;
    amount max_block_size = max_node->block->capacity;
    if (size_to_occupy > max_block_size) return false;
    return true;
}

BlockNode *block_heap_occupy_max(amount size_to_occupy, BlockHeap *block_heap) {
    if (!block_heap_can_occupy_max(size_to_occupy, block_heap)) return NULL;

    BlockNode *max_node = block_heap_peek(block_heap);

    // Случай №1. Блок можно разделить на два блока с положительными size
    // Длина занятого блока: size_to_occupy
    // Длина оставшегося блока: block_content_size - size_to_occupy - block_header_size
    if (max_node->block->capacity > size_to_occupy + block_header_size()) {
        Block *occupied_block = block_init();
        occupied_block->is_free = false;
        occupied_block->size = occupied_block->capacity = size_to_occupy;
        occupied_block->content = malloc(occupied_block->size * sizeof(*occupied_block->content));

        BlockNode *occupied_node = block_node_init(occupied_block);
        occupied_node->block_pointer = max_node->block_pointer;
        occupied_node->next = max_node;

        max_node->block->capacity -= size_to_occupy + block_header_size();
        max_node->block->size -= size_to_occupy + block_header_size();
        max_node->block_pointer += block_size(occupied_block);
        block_heap_sift_down(0, block_heap);

        return occupied_node;
    }

    // Случай №2. Блок нельзя разделить
    max_node->block->size = size_to_occupy;
    max_node->block->is_free = false;

    swap_indexes(0, block_heap_size(block_heap) - 1, block_heap);

    BlockNode *occupied = block_node_vector_pop(block_heap->block_node_vector);
    block_heap_sift_down(0, block_heap);

    return occupied;
}
