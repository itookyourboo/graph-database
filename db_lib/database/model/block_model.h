#ifndef LOW_LEVEL_PROGRAMMING_TASK1_BLOCK_MODEL_H
#define LOW_LEVEL_PROGRAMMING_TASK1_BLOCK_MODEL_H

#include <stdint.h>
#include "../mem.h"

typedef struct __attribute__((packed)) {
    uint8_t is_free;
    amount size;
    amount capacity;
    byte *content;
} Block;

Block *block_init();
amount block_header_size();
amount block_size(Block *block);
void block_free(Block *block);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_BLOCK_MODEL_H
