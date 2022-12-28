#ifndef LOW_LEVEL_PROGRAMMING_TASK1_DATABASE_MODEL_H
#define LOW_LEVEL_PROGRAMMING_TASK1_DATABASE_MODEL_H

#include "../mem.h"

typedef struct __attribute__((packed)) {
    pointer schema_first;
    pointer schema_last;

    pointer node_first;
    pointer node_last;

    pointer link_first;
    pointer link_last;
} DB_Header;

DB_Header *db_header_init();
amount db_header_size(DB_Header *db_header);
void db_header_free(DB_Header *db_header);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_DATABASE_MODEL_H
