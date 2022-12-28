#include "util.h"

RC change_pointer(size_t offset, pointer object_pointer, pointer new_pointer, Connection *connection) {
    MV_SAVE(object_pointer);
    Block *block = block_init();
    block_from_file(block, connection);

    pointer end_of_block = ftell(connection->fp);
    end_of_block -= block->capacity - block->size;
    end_of_block -= offset * sizeof(pointer);

    connection_move_to_pointer(end_of_block, connection);

    if (fwrite(&new_pointer, sizeof(new_pointer), 1, connection->fp) != 1) {
        return RC_DATABASE_WRITE_DATABASE_HEADER_ERROR;
    }

    MV_BACK();
    return RC_OK;
}

RC change_prev_pointer(pointer object_pointer, pointer prev_pointer, Connection *connection) {
    return change_pointer(2, object_pointer, prev_pointer, connection);
}

RC change_next_pointer(pointer object_pointer, pointer next_pointer, Connection *connection) {
    return change_pointer(1, object_pointer, next_pointer, connection);
}