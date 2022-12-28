#ifndef LOW_LEVEL_PROGRAMMING_TASK1_EXCEPTIONS_H
#define LOW_LEVEL_PROGRAMMING_TASK1_EXCEPTIONS_H

#include <stdlib.h>

#define throw_if_not_ok(rc) do { \
if (rc != RC_OK)                 \
    exit(rc);                   \
} while(0)

#define throw(rc) do { \
    exit(rc);\
} while(0)\

// RC - Return Code
typedef enum {
    RC_OK = 0,
    RC_DATABASE_EXISTS,
    RC_DATABASE_CREATED,
    RC_DATABASE_CANNOT_CREATE,
    RC_DATABASE_WRITE_DATABASE_HEADER_ERROR,
    RC_DATABASE_CANNOT_OPEN,
    RC_DATABASE_READ_DATABASE_HEADER_ERROR,
    RC_DATABASE_CANNOT_CLOSE,
    RC_DATABASE_NO_SUCH_SCHEMA,
    RC_DATA_TYPES_ARE_NOT_EQUAL,
    RC_DATABASE_ATTRIBUTE_NOT_FOUND,
    RC_SCHEMA_NAME_CANT_BE_NULL
} RC;

#endif //LOW_LEVEL_PROGRAMMING_TASK1_EXCEPTIONS_H