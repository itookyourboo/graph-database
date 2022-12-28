#ifndef LOW_LEVEL_PROGRAMMING_TASK1_DATABASE_MANAGER_H
#define LOW_LEVEL_PROGRAMMING_TASK1_DATABASE_MANAGER_H

#ifdef WIN32
#include <io.h>
#define F_OK 0
#define access _access
#endif

#include "exceptions.h"
#include "connection.h"
#include "model/string_model.h"
#include "model/attribute_model.h"
#include "model/schema_model.h"

RC database_open(char *filename, Connection *connection);
RC database_close(Connection *connection);
RC database_read_header(Connection *connection);
RC database_write_header(Connection *connection);
RC database_update_header(Connection *connection);
RC database_free_blocks(pointer block_pointers[], amount block_num, Connection *connection);

BlockNode *database_get_insertion_block(amount size, Connection *connection);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_DATABASE_MANAGER_H
