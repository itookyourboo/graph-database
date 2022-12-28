#ifndef LOW_LEVEL_PROGRAMMING_TASK1_CONNECTION_H
#define LOW_LEVEL_PROGRAMMING_TASK1_CONNECTION_H

#include <stdio.h>
#include "buffer_manager.h"
#include "exceptions.h"

typedef struct {
    char *filename;
    FILE *fp;
    BufferManager *buffer_manager;
} Connection;

RC connection_open(char *filename, Connection *connection);
RC connection_close(Connection *connection);
RC connection_move_to_pointer(pointer ptr, Connection *connection);

#define SAVE()                                                          \
pointer save = ftell(connection->fp)

#define MV_SAVE(ptr)                                                    \
pointer save = ftell(connection->fp);                                   \
connection_move_to_pointer(ptr, connection)

#define MV_BACK()                                                       \
connection_move_to_pointer(save, connection)

#define BACK() MV_BACK()

#endif //LOW_LEVEL_PROGRAMMING_TASK1_CONNECTION_H
