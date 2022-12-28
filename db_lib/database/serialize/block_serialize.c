#include <string.h>
#include <malloc.h>

#include "util.h"
#include "block_serialize.h"


void seek_padding(Block *block, Connection *connection) {
    fseek(connection->fp, block->capacity - block->size, SEEK_CUR);
}


RC block_to_bytes(Block *block, byte *bytes) {
    amount i = 0;
    printf("%d %d\n", block->is_free, block->size);
    field_to_bytes(block, is_free, bytes, i);
    field_to_bytes(block, size, bytes, i);
    field_to_bytes(block, capacity, bytes, i);
    field_array_to_bytes(block, content, block->size, bytes, i);
    return RC_OK;
}

RC block_from_bytes(Block *block, byte *bytes) {
    amount i = 0;
    field_from_bytes(block, is_free, bytes, i);
    field_from_bytes(block, size, bytes, i);
    field_from_bytes(block, capacity, bytes, i);
    field_array_from_bytes(block, content, block->size, bytes, i);
    return RC_OK;
}

RC block_to_file(Block *block, Connection *connection) {
    write_field(block, is_free, connection->fp);
    write_field(block, size, connection->fp);
    write_field(block, capacity, connection->fp);
    if (block->content != NULL)
        write_array(block, block->size, content, connection->fp);
    else
        write_array_zeros(block->size, connection->fp);
    seek_padding(block, connection);
    return RC_OK;
}

RC block_from_file(Block *block, Connection *connection) {
    read_field(block, is_free, connection->fp);
    read_field(block, size, connection->fp);
    read_field(block, capacity, connection->fp);
    read_array(block, block->size, content, connection->fp);
    seek_padding(block, connection);
    return RC_OK;
}

RC block_node_save(BlockNode *block_node, Connection *connection) {
    MV_SAVE(block_node->block_pointer);
    RC rc = block_to_file(block_node->block, connection);
    MV_BACK();
    return rc;
}

