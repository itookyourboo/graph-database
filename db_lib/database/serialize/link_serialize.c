#include "link_serialize.h"
#include "util.h"

RC db_link_to_bytes(DB_Link *db_link, byte *bytes) {
    amount i = 0;
    field_to_bytes(db_link, link_name_string_pointer, bytes, i);
    field_to_bytes(db_link, link_type, bytes, i);
    field_to_bytes(db_link, node_first_pointer, bytes, i);
    field_to_bytes(db_link, node_second_pointer, bytes, i);
    field_to_bytes(db_link, link_prev_pointer, bytes, i);
    field_to_bytes(db_link, link_next_pointer, bytes, i);
    return RC_OK;
}

RC db_link_from_bytes(DB_Link *db_link, byte *bytes) {
    amount i = 0;
    field_from_bytes(db_link, link_name_string_pointer, bytes, i);
    field_from_bytes(db_link, link_type, bytes, i);
    field_from_bytes(db_link, node_first_pointer, bytes, i);
    field_from_bytes(db_link, node_second_pointer, bytes, i);
    field_from_bytes(db_link, link_prev_pointer, bytes, i);
    field_from_bytes(db_link, link_next_pointer, bytes, i);
    return RC_OK;
}
