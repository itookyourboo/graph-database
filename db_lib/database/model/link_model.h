#ifndef LOW_LEVEL_PROGRAMMING_TASK1_LINK_MODEL_H
#define LOW_LEVEL_PROGRAMMING_TASK1_LINK_MODEL_H

#include "../mem.h"
#include "value_model.h"
#include "node_model.h"


typedef enum {
    LT_DIRECTED_TO,
    LT_DIRECTED_FROM,
    LT_DIRECTED_BOTH,

    LT_ANY,
} LinkType;


typedef struct __attribute__((packed)) {
    pointer link_name_string_pointer;
    LinkType link_type;

    pointer node_first_pointer;
    pointer node_second_pointer;

    pointer link_prev_pointer;
    pointer link_next_pointer;
} DB_Link;

DB_Link *db_link_init();
amount db_link_size(DB_Link *db_link);
void db_link_free(DB_Link *db_link);

typedef struct {
    String *name;
    LinkType type;

    pointer _db_ptr;
    pointer _link_prev_ptr;
    pointer _link_next_ptr;
} Link;

Link *link_init();
amount link_size(Link *link);
void link_free(Link *link);

Link *link_copy(Link *src);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_LINK_MODEL_H
