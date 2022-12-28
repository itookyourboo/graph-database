#include "attribute_serialize.h"
#include "util.h"
#include "block_serialize.h"
#include "string_serialize.h"


RC db_attribute_to_bytes(DB_Attribute *db_attribute, byte *bytes) {
    amount i = 0;
    field_to_bytes(db_attribute, name_string_pointer, bytes, i);
    field_to_bytes(db_attribute, data_type, bytes, i);
    return RC_OK;
}

RC db_attribute_from_bytes(DB_Attribute *db_attribute, byte *bytes) {
    amount i = 0;
    field_from_bytes(db_attribute, name_string_pointer, bytes, i);
    field_from_bytes(db_attribute, data_type, bytes, i);
    return RC_OK;
}

RC db_attribute_to_attribute(DB_Attribute *db_attribute, Attribute *attribute, Connection *connection) {
    connection_move_to_pointer(db_attribute->name_string_pointer, connection);
    Block *block_string_name = block_init();
    block_from_file(block_string_name, connection);
    attribute->name = string_init();
    string_from_bytes(attribute->name, block_string_name->content);
    attribute->data_type = db_attribute->data_type;
}

RC db_attribute_from_attribute(DB_Attribute *db_attribute, Attribute *attribute) {

}