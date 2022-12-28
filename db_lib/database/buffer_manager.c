#include <malloc.h>
#include "buffer_manager.h"


BufferManager *buffer_manager_init() {
    BufferManager *buffer_manager = malloc(sizeof(BufferManager));
    buffer_manager->free_blocks = block_heap_init(NULL);
    buffer_manager->db_header = db_header_init();
    return buffer_manager;
}

amount buffer_manager_size(BufferManager *buffer_manager) {
    return sizeof(BufferManager);
}

void buffer_manager_free(BufferManager *buffer_manager) {
    db_header_free(buffer_manager->db_header);
    block_heap_free(buffer_manager->free_blocks);
    free(buffer_manager);
}