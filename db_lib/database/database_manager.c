#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "database_manager.h"
#include "mem.h"
#include "visualize.h"
#include "serialize/block_serialize.h"
#include "data_structure/block_heap.h"


bool database_exists(char *database_filename) {
    return access(database_filename, F_OK) == 0;
}

RC database_create_file_if_not_exists(char *filename) {
    if (database_exists(filename))
        return RC_DATABASE_EXISTS;
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
        return RC_DATABASE_CANNOT_CREATE;
    if (fclose(fp))
        return RC_DATABASE_CANNOT_CLOSE;
    return RC_DATABASE_CREATED;
}

RC database_read_header(Connection *connection) {
    DB_Header *db_header = connection->buffer_manager->db_header;
    fseek(connection->fp, 0, SEEK_SET);
    if (fread(db_header, db_header_size(db_header), 1, connection->fp) != 1)
        return RC_DATABASE_READ_DATABASE_HEADER_ERROR;
    return RC_OK;
}

RC database_write_header(Connection *connection) {
    DB_Header *db_header = connection->buffer_manager->db_header;
    fseek(connection->fp, 0, SEEK_SET);
    if (fwrite(db_header, db_header_size(db_header), 1, connection->fp) != 1)
        return RC_DATABASE_WRITE_DATABASE_HEADER_ERROR;
    return RC_OK;
}

RC database_update_header(Connection *connection) {
    SAVE();
    RC rc = database_write_header(connection);
    BACK();
    return rc;
}

RC database_init(Connection *connection) {
    fseek(connection->fp, 0, SEEK_END);
    pointer file_size = ftell(connection->fp);
    fseek(connection->fp, 0, SEEK_SET);

    database_read_header(connection);

    pointer block_pointer;
    BlockHeap *free_blocks = connection->buffer_manager->free_blocks;
    while ((block_pointer = ftell(connection->fp)) < file_size) {
        Block *block = block_init();
        block_from_file(block, connection);
        if (!block->is_free) continue;

        BlockNode *block_node = block_node_init(block);
        block_node->block_pointer = block_pointer;
        block_heap_push(block_node, free_blocks);
    }

//    printf("Free blocks:\n");
//    printf("\n");
    block_heap_build(free_blocks);
//    block_heap_print(free_blocks);

    return RC_OK;
}

pointer database_get_end_pointer(Connection *connection) {
    SAVE();

    pointer result = nullptr;
    fseek(connection->fp, 0, SEEK_END);
    result = ftell(connection->fp);

    BACK();
    return result;
}

BlockNode *database_get_insertion_block(amount size, Connection *connection) {
    BlockHeap *free_blocks = connection->buffer_manager->free_blocks;
    if (block_heap_can_occupy_max(size, free_blocks))
        return block_heap_occupy_max(size, free_blocks);

    Block *block = block_init();
    block->is_free = false;
    block->size = block->capacity = size;
    BlockNode *block_node = block_node_init(block);
    block_node->block_pointer = database_get_end_pointer(connection);
    return block_node;
}

void database_update_free_blocks_heap(Connection *connection) {
    block_heap_build(connection->buffer_manager->free_blocks);
}

RC database_free_blocks(pointer block_pointers[], amount block_num, Connection *connection) {
    RC rc = RC_OK;

    for (amount i = 0; i < block_num; i++) {
        connection_move_to_pointer(block_pointers[i], connection);
        throw_if_not_ok(rc);
        if (fwrite(&(bool){true}, sizeof(bool), 1, connection->fp) != 1) {
            return RC_DATABASE_CREATED;
        }
        connection_move_to_pointer(block_pointers[i], connection);
        Block *block = block_init();
        BlockNode *block_node = block_node_init(block);
        block_node->block_pointer = block_pointers[i];
        block_from_file(block, connection);
        block_heap_push(block_node, connection->buffer_manager->free_blocks);
    }

    database_update_free_blocks_heap(connection);

    return rc;
}

RC database_open(char *filename, Connection *connection) {
    RC create_status = database_create_file_if_not_exists(filename);
    if (create_status == RC_DATABASE_CANNOT_CREATE || create_status == RC_DATABASE_CANNOT_CLOSE)
        return create_status;
    RC rc = connection_open(filename, connection);
    throw_if_not_ok(rc);
    if (create_status == RC_DATABASE_CREATED)
        database_write_header(connection);
    rc = database_init(connection);
    throw_if_not_ok(rc);
    return rc;
}

RC database_close(Connection *connection) {
    return connection_close(connection);
}
