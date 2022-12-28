#ifndef LOW_LEVEL_PROGRAMMING_TASK1_LINK_CRUD_H
#define LOW_LEVEL_PROGRAMMING_TASK1_LINK_CRUD_H

#include "crud_util.h"
#include "../condition.h"
#include "../data_structure/vector.h"

DECLARE_INSERT_FUNCTION(DB_Link, db_link)
DECLARE_RETRIEVE_FUNCTION(DB_Link, db_link)
DECLARE_UPDATE_FUNCTION(DB_Link, db_link)

RC link_create(
        Link *link,
        Node *first,
        Node *second,
        Connection *connection
);
RC link_delete(
        LinkVector *link_vector,
        Connection *connection
);
RC link_update(
        LinkVector *link_vector,
        String new_name,
        LinkType new_link_type,
        Connection *connection
);
Generator *match(
        MatchCondition condition,
        Return return_items,
        Connection *connection
);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_LINK_CRUD_H
