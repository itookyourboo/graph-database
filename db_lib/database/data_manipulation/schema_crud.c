#include "schema_crud.h"
#include "string_crud.h"
#include "attribute_crud.h"
#include "node_crud.h"
#include "../serialize/schema_serialize.h"
#include "../serialize/util.h"

DEFINE_INSERT_FUNCTION(DB_Schema, db_schema)

RC schema_create(Schema *schema, Connection *connection) {
    RC rc = RC_OK;

    DB_Header *db_header = connection->buffer_manager->db_header;

    pointer block_string_pointer;
    rc = string_insert(schema->name, &block_string_pointer, connection);
    throw_if_not_ok(rc);

    DB_Schema *db_schema = db_schema_init();
    db_schema_from_schema(db_schema, schema);
    db_schema->name_string_pointer = block_string_pointer;

    for (amount i = 0; i < db_schema->attr_count; i++) {
        rc = attribute_insert(schema->attributes[i], &db_schema->attr_pointers[i], connection);
        throw_if_not_ok(rc);
    }

    pointer schema_pointer;
    rc = db_schema_insert(db_schema, &schema_pointer, connection);
    throw_if_not_ok(rc);

    if (db_header->schema_first == nullptr) {
        db_header->schema_first = schema_pointer;
    } else {
        rc = change_next_pointer(
                db_header->schema_last,
                schema_pointer,
                connection
        );
        throw_if_not_ok(rc);
    }
    db_header->schema_last = schema_pointer;
    rc = database_update_header(connection);
    throw_if_not_ok(rc);

    db_schema_free(db_schema);

    schema->_db_ptr = schema_pointer;

    return rc;
}

RC schema_find(char *name, pointer *schema_pointer, DB_Schema *db_schema_found, Connection *connection) {
    RC rc = RC_DATABASE_NO_SUCH_SCHEMA;

    DB_Header *db_header = connection->buffer_manager->db_header;
    pointer current_schema_pointer = db_header->schema_first;
    if (current_schema_pointer == nullptr)
        return rc;

    DB_Schema *db_schema = db_schema_init();
    Block *block = block_init();
    String *schema_name = string_init();

    while (current_schema_pointer != nullptr) {
        connection_move_to_pointer(current_schema_pointer, connection);

        block_from_file(block, connection);
        db_schema_from_bytes(db_schema, block->content);

        MV_SAVE(db_schema->name_string_pointer);
        string_retrieve(schema_name, connection);
        MV_BACK();

        if (strcmp(name, schema_name->content) == 0) {
            *schema_pointer = current_schema_pointer;
            memcpy(db_schema_found, db_schema, sizeof(*db_schema));
            rc = RC_OK;
            break;
        }
        current_schema_pointer = db_schema->schema_next_pointer;
    }

    block_free(block);
    string_free(schema_name);

    return rc;
}

RC schema_show(char *name, Schema *schema, Connection *connection) {
    pointer schema_pointer;
    DB_Schema *db_schema = db_schema_init();
    RC rc = schema_find(name, &schema_pointer, db_schema, connection);
    throw_if_not_ok(rc);
    rc = db_schema_to_schema(db_schema, schema, connection);
    db_schema_free(db_schema);
    schema->_db_ptr = schema_pointer;
    return rc;
}

RC schema_delete(char *name, Connection *connection) {
    RC rc = RC_DATABASE_NO_SUCH_SCHEMA;
    DB_Header *db_header = connection->buffer_manager->db_header;
    pointer schema_pointer = db_header->schema_first;
    if (schema_pointer == nullptr)
        return rc;

    Schema *schema = schema_init();
    pointer previous_schema_pointer = nullptr;

    DB_Schema *db_schema;
    Block *block;

    while (schema_pointer != nullptr) {
        connection_move_to_pointer(schema_pointer, connection);

        db_schema = db_schema_init();
        block = block_init();
        rc = block_from_file(block, connection);
        throw_if_not_ok(rc);
        rc = db_schema_from_bytes(db_schema, block->content);
        throw_if_not_ok(rc);
        rc = db_schema_to_schema(db_schema, schema, connection);
        throw_if_not_ok(rc);
        if (strcmp(name, schema->name->content) == 0) {
            if (previous_schema_pointer == nullptr) {
                db_header->schema_first = db_schema->schema_next_pointer;
            } else {
                rc = change_next_pointer(
                        previous_schema_pointer,
                        db_schema->schema_next_pointer,
                        connection
                );
                throw_if_not_ok(rc);
            }
            if (db_header->schema_last == schema_pointer)
                db_header->schema_last = nullptr;

            amount block_num = 2 * db_schema->attr_count + 2;
            pointer block_pointers_to_free[block_num];
            amount i, j;

            DB_Attribute *db_attribute = db_attribute_init();
            for (i = 0, j = 0; j < db_schema->attr_count; j++, i += 2) {
                connection_move_to_pointer(db_schema->attr_pointers[j], connection);
                db_attribute_retrieve(db_attribute, connection);
                block_pointers_to_free[i] = db_schema->attr_pointers[j];
                block_pointers_to_free[i + 1] = db_attribute->name_string_pointer;
            }
            block_pointers_to_free[i++] = db_schema->name_string_pointer;
            block_pointers_to_free[i++] = schema_pointer;
            database_free_blocks(block_pointers_to_free, block_num, connection);

            db_attribute_free(db_attribute);
            rc = RC_OK;
            break;
        }

        previous_schema_pointer = schema_pointer;
        schema_pointer = db_schema->schema_next_pointer;
    }

    schema_free(schema);
    db_schema_free(db_schema);
    block_free(block);

    database_update_header(connection);
    return rc;
}

typedef struct {
    pointer prev;
    pointer cur;
    Connection *connection;
} ResultContext;

Schema *schema_next(Generator *generator) {
    ResultContext *context = generator->context;
    Schema *schema = generator->result;

    pointer current_schema_pointer;
    Connection *connection = context->connection;

    DB_Schema *db_schema = db_schema_init();
    Block *block = block_init();
    String *schema_name = string_init();

    RC rc;

    while (context->cur != nullptr) {
        current_schema_pointer = context->cur;
        connection_move_to_pointer(current_schema_pointer, connection);

        block_from_file(block, connection);
        db_schema_from_bytes(db_schema, block->content);

        MV_SAVE(db_schema->name_string_pointer);
        string_retrieve(schema_name, connection);
        MV_BACK();

        db_schema_to_schema(db_schema, schema, connection);

        context->prev = current_schema_pointer;
        context->cur = db_schema->schema_next_pointer;

        return generator->result;
    }

    db_schema_free(db_schema);
    block_free(block);
    string_free(schema_name);

    return (generator->result = NULL);
}


Generator *all_schemas(
        Connection *connection
) {
    Generator *generator = generator_init();
    generator->result = schema_init();

    ResultContext *context = malloc(sizeof(ResultContext));
    generator->context = context;

    context->cur = connection->buffer_manager->db_header->schema_first;
    context->prev = nullptr;
    context->connection = connection;

    generator->next_function = (void *(*)(Generator *)) schema_next;

    return generator;
}
