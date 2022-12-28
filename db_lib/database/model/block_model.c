#include <malloc.h>
#include "block_model.h"


Block *block_init() {
    Block *block = malloc(sizeof(Block));
    block->content = NULL;
    return block;
}

amount block_header_size() {
    Block block = {0};
    return (
            sizeof(block.is_free) +
            sizeof(block.size) +
            sizeof(block.capacity)
    );
}

amount block_size(Block *block) {
    return (
            sizeof(block->is_free) +
            sizeof(block->size) +
            sizeof(block->capacity) +
            block->size * sizeof(*block->content)
    );
}

void block_free(Block *block) {
    if (block->content != NULL)
        free(block->content);
    free(block);
}
