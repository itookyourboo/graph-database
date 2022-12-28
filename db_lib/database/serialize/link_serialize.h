#ifndef LOW_LEVEL_PROGRAMMING_TASK1_LINK_SERIALIZE_H
#define LOW_LEVEL_PROGRAMMING_TASK1_LINK_SERIALIZE_H

#include "../exceptions.h"
#include "../connection.h"
#include "../model/link_model.h"

RC db_link_to_bytes(DB_Link *db_link, byte *bytes);
RC db_link_from_bytes(DB_Link *db_link, byte *bytes);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_LINK_SERIALIZE_H
