#include "attribute_crud.h"
#include "string_crud.h"

DEFINE_INSERT_FUNCTION(DB_Attribute, db_attribute)
DEFINE_RETRIEVE_FUNCTION(DB_Attribute, db_attribute)

RC attribute_insert(Attribute *attribute, pointer *insertion_pointer, Connection *connection) {
    RC rc = RC_OK;
    pointer block_string_pointer;

    rc = string_insert(attribute->name, &block_string_pointer, connection);
    throw_if_not_ok(rc);

    DB_Attribute *db_attribute = &(DB_Attribute) {
            .data_type = attribute->data_type,
            .name_string_pointer = block_string_pointer
    };

    rc = db_attribute_insert(db_attribute, insertion_pointer, connection);
    throw_if_not_ok(rc);
    return rc;
}
