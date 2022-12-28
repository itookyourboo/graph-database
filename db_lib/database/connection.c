#include <stdio.h>
#include <malloc.h>

#include "connection.h"

RC connection_open(char *filename, Connection *connection) {
    connection->filename = filename;
    connection->fp = fopen(filename, "rb+");
    connection->buffer_manager = buffer_manager_init();
    if (connection->fp == NULL)
        return RC_DATABASE_CANNOT_OPEN;
    return RC_OK;
}

RC connection_close(Connection *connection) {
    if (fclose(connection->fp))
        return RC_DATABASE_CANNOT_CLOSE;
    connection->filename = NULL;
    connection->fp = NULL;
    buffer_manager_free(connection->buffer_manager);

    return RC_OK;
}

RC connection_move_to_pointer(pointer ptr, Connection *connection) {
    fseek(connection->fp, ptr, SEEK_SET);
}
