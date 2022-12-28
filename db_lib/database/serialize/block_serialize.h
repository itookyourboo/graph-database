#ifndef LOW_LEVEL_PROGRAMMING_TASK1_BLOCK_SERIALIZE_H
#define LOW_LEVEL_PROGRAMMING_TASK1_BLOCK_SERIALIZE_H

#include "../exceptions.h"
#include "../model/block_model.h"
#include "../mem.h"
#include "../connection.h"

RC block_to_bytes(Block *block, byte *bytes);
RC block_from_bytes(Block *block, byte *bytes);

RC block_to_file(Block *block, Connection *connection);
RC block_from_file(Block *block, Connection *connection);

RC block_node_save(BlockNode *block_node, Connection *connection);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_BLOCK_SERIALIZE_H
