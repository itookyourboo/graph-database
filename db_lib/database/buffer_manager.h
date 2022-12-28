#ifndef LOW_LEVEL_PROGRAMMING_TASK1_BUFFER_MANAGER_H
#define LOW_LEVEL_PROGRAMMING_TASK1_BUFFER_MANAGER_H

#include "data_structure/block_heap.h"
#include "model/database_model.h"

typedef struct {
    DB_Header *db_header;
    BlockHeap *free_blocks;
} BufferManager;

BufferManager *buffer_manager_init();
amount buffer_manager_size(BufferManager *buffer_manager);
void buffer_manager_free(BufferManager *buffer_manager);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_BUFFER_MANAGER_H
