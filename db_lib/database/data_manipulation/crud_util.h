#ifndef LOW_LEVEL_PROGRAMMING_TASK1_CRUD_UTIL_H
#define LOW_LEVEL_PROGRAMMING_TASK1_CRUD_UTIL_H


#include "../exceptions.h"
#include "../connection.h"
#include "../serialize/serializers.h"
#include "../model/models.h"
#include "../database_manager.h"


#define DECLARE_INSERT_FUNCTION(Type, type)                                                     \
pointer type##_insert(Type *type, pointer *insertion_pointer, Connection *connection);

#define DECLARE_RETRIEVE_FUNCTION(Type, type)                                                   \
RC type##_retrieve(Type *type, Connection *connection);

#define DECLARE_UPDATE_FUNCTION(Type, type)                                                     \
RC type##_update(Type *type, Connection *connection);

#define DEFINE_INSERT_FUNCTION(Type, type)                                                      \
pointer type##_insert(Type *type, pointer *insertion_pointer, Connection *connection) {         \
    amount size = type##_size(type);                                                            \
    BlockNode *block_node = database_get_insertion_block(size, connection);                     \
    *insertion_pointer = block_node->block_pointer;                                             \
    connection_move_to_pointer(block_node->block_pointer, connection);                          \
    byte *bytes = malloc(size);                                                                 \
    type##_to_bytes(type, bytes);                                                               \
    block_node->block->content = bytes;                                                         \
    block_to_file(block_node->block, connection);                                               \
    return RC_OK;                                                                               \
}                                                                                               \

#define DEFINE_RETRIEVE_FUNCTION(Type, type)                                                    \
RC type##_retrieve(Type *type, Connection *connection) {                                        \
    RC rc = RC_OK;                                                                              \
    Block block = {};                                                                           \
    rc = block_from_file(&block, connection);                                                   \
    throw_if_not_ok(rc);                                                                        \
    rc = type##_from_bytes(type, block.content);                                                \
    return rc;                                                                                  \
}

#define DEFINE_UPDATE_FUNCTION(Type, type)                                                      \
RC type##_update(Type *type, Connection *connection) {                                          \
    Block *block = block_init();                                                                \
    SAVE();                                                                                     \
    block_from_file(block, connection);                                                         \
    BACK();                                                                                     \
    type##_to_bytes(type, block->content);                                                      \
    block_to_file(block, connection);                                                           \
    block_free(block);                                                                          \
}

#endif //LOW_LEVEL_PROGRAMMING_TASK1_CRUD_UTIL_H
