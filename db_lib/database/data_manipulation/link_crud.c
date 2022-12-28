#include "node_crud.h"
#include "link_crud.h"
#include "schema_crud.h"
#include "string_crud.h"


DEFINE_INSERT_FUNCTION(DB_Link, db_link)

DEFINE_RETRIEVE_FUNCTION(DB_Link, db_link)

DEFINE_UPDATE_FUNCTION(DB_Link, db_link)


RC link_create(
        Link *link,
        Node *first,
        Node *second,
        Connection *connection
) {
    RC rc = RC_OK;
    DB_Header *db_header = connection->buffer_manager->db_header;

    pointer string_insert_pointer;
    rc = string_insert(link->name, &string_insert_pointer, connection);
    throw_if_not_ok(rc);

    DB_Link *db_link = &(DB_Link) {
            .link_name_string_pointer = string_insert_pointer,
            .link_type = link->type,
            .node_first_pointer = first->_db_ptr,
            .node_second_pointer = second->_db_ptr,
            .link_prev_pointer = db_header->link_last,
            .link_next_pointer = nullptr
    };

    pointer link_insert_pointer;
    rc = db_link_insert(db_link, &link_insert_pointer, connection);
    throw_if_not_ok(rc);

    if (db_header->link_first == nullptr) {
        db_header->link_first = link_insert_pointer;
    } else {
        rc = change_next_pointer(
                db_header->link_last,
                link_insert_pointer,
                connection
        );
        throw_if_not_ok(rc);
    }
    db_header->link_last = link_insert_pointer;
    database_update_header(connection);
    return rc;
}

#define SKIP do{                            \
context->prev = context->cur;               \
context->cur = db_link->link_next_pointer;  \
goto loop;                                  \
} while(0)

bool check_link_condition(
        DB_Link *db_link,
        LinkCondition link_condition,
        Connection *connection
) {
    if (link_condition.is_null)
        return true;

    if (
            link_condition.link_type != LT_ANY &&
            link_condition.link_type != db_link->link_type
    ) {
        return false;
    }

    SAVE();

    bool result = true;
    String *link_name = string_init();
    connection_move_to_pointer(db_link->link_name_string_pointer, connection);
    string_retrieve(link_name, connection);

    if (
            link_condition.link_name != NULL &&
            string_string_cmp(link_condition.link_name, link_name) != 0
    )
        result = false;

    BACK();
    return result;
}


typedef struct {
    pointer prev;
    pointer cur;
    Return flags;
    Generator *gen1;
    Generator *gen2;
    Connection *connection;
    MatchCondition condition;
} ResultContext;


Result *link_next(Generator *generator) {
    ResultContext *context = generator->context;
    MatchCondition condition = context->condition;
    Connection *connection = context->connection;
    Return flags = context->flags;
    Result *result = generator->result;

    LinkCondition link_c = condition.link_condition;
    NodeCondition node1_c = condition.node_first_condition;
    NodeCondition node2_c = condition.node_second_condition;
    NodeCondition cross_c = condition.node_cross_condition;

    if (condition.is_null) {
        if (flags == RETURN_NOTHING) {
            return NULL;
        }

        // Декартово произведение
        if (flags == (RETURN_NODE_FIRST | RETURN_NODE_SECOND)) {
            Node *node1, *node2;
            if (context->gen1 == NULL) {
                context->gen1 = match_nodes(node1_c, connection);
                node1 = generator_next(context->gen1);

                context->gen2 = match_nodes(node2_c, connection);
                node2 = generator_next(context->gen2);

                if (node1 == NULL || node2 == NULL)
                    return NULL;

                result->first = node1;
                result->second = node2;

                return result;
            } else {
                node1 = context->gen1->result;
                node2 = context->gen2->result;
            }

            if (node1 == NULL) {
                // Прошли по всем первым нодам
                return NULL;
            }

            node2 = generator_next(context->gen2);
            if (node2 == NULL) {
                // Сделали один цикл для первой ноды
                context->gen2 = match_nodes(node2_c, connection);
                node2 = generator_next(context->gen2);
                node1 = generator_next(context->gen1);
            }

            if (node1 == NULL)
                return NULL;

            result->first = node1;
            result->second = node2;

            return result;
        }

        if (flags == RETURN_NODE_FIRST) {
            if (context->gen1 == NULL)
                context->gen1 = match_nodes(node1_c, connection);
            result->first = generator_next(context->gen1);
            if (result->first == NULL)
                return NULL;
        } else if (flags == RETURN_NODE_SECOND) {
            if (context->gen2 == NULL)
                context->gen2 = match_nodes(node2_c, connection);
            result->second = generator_next(context->gen2);
            if (result->first == NULL)
                return NULL;
        }
        return result;
    }

    DB_Link *db_link = db_link_init();

    DB_Node *db_node_first = db_node_init(), *db_node_second = db_node_init();
    Block *block = block_init();

    loop:
    while (context->cur != nullptr) {
        connection_move_to_pointer(context->cur, connection);
        db_link_retrieve(db_link, connection);

        if (!check_link_condition(db_link, link_c, connection))
            SKIP;

        connection_move_to_pointer(db_link->node_first_pointer, connection);
        block_from_file(block, connection);
        db_node_from_bytes(db_node_first, block->content);

        if (!check_node_condition(db_node_first, node1_c, connection))
            SKIP;

        connection_move_to_pointer(db_link->node_second_pointer, connection);
        block_from_file(block, connection);
        db_node_from_bytes(db_node_second, block->content);

        if (!check_node_condition(db_node_second, node2_c, connection))
            SKIP;

        if (!check_cross_node_condition(node1_c.is_null ? NULL : node1_c.schema, db_node_first,
                                        node2_c.is_null ? NULL : node2_c.schema, db_node_second,
                                        cross_c, connection))
            SKIP;

        if (flags & RETURN_NODE_FIRST) {
            Node *node = node_init();
            node->_db_ptr = db_link->node_first_pointer;
            db_node_to_node(db_node_first, node, connection);

            result->first = node;
        }
        if (flags & RETURN_NODE_SECOND) {
            Node *node = node_init();
            node->_db_ptr = db_link->node_second_pointer;
            db_node_to_node(db_node_second, node, connection);

            result->second = node;
        }
        if (flags & RETURN_LINK) {
            Link *link = link_init();
            String *link_name = string_init();

            connection_move_to_pointer(db_link->link_name_string_pointer, connection);
            string_retrieve(link_name, connection);

            link->type = db_link->link_type;
            link->name = link_name;

            link->_db_ptr = context->cur;
            link->_link_prev_ptr = db_link->link_prev_pointer;
            link->_link_next_ptr = db_link->link_next_pointer;

            result->link = link;
        }

        context->prev = context->cur;
        context->cur = db_link->link_next_pointer;

        return result;
    }

    db_link_free(db_link);
    db_node_free(db_node_first);
    db_node_free(db_node_second);
    block_free(block);

    return (generator->result = NULL);
}

Generator *match(
        MatchCondition condition,
        Return return_items,
        Connection *connection
) {
    Generator *generator = generator_init();

    if (condition.is_null && (return_items & RETURN_LINK)) {
        condition.is_null = false;
        condition.link_condition = (LinkCondition){
                .link_name = NULL,
                .link_type = LT_ANY
        };
    }

    size_t count = return_count(return_items);
    Result *result = result_init(count);
    generator->result = result;

    result->return_items = return_items;

    ResultContext *context = malloc(sizeof(ResultContext));
    generator->context = context;

    context->flags = return_items;
    context->condition = condition;
    context->connection = connection;
    context->cur = connection->buffer_manager->db_header->link_first;
    context->gen1 = context->gen2 = NULL;

    generator->next_function = (void *(*)(Generator *)) link_next;
    return generator;
}

#define LINK_DELETE_BLOCKS_NUM 2 // string and link blocks

RC link_delete(
        LinkVector *link_vector,
        Connection *connection
) {
    RC rc = RC_OK;

    Link *link;
    DB_Link *db_link = db_link_init();
    DB_Header *db_header = connection->buffer_manager->db_header;

    V_ITERATE(link_vector, i) {
        link = link_vector_get(i, link_vector);

        connection_move_to_pointer(link->_db_ptr, connection);
        db_link_retrieve(db_link, connection);

        if (link->_db_ptr == db_header->link_first)
            db_header->link_first = db_link->link_next_pointer;

        if (db_link->link_next_pointer != nullptr)
            change_prev_pointer(db_link->link_next_pointer, db_link->link_prev_pointer, connection);
        if (db_link->link_prev_pointer != nullptr)
            change_next_pointer(db_link->link_prev_pointer, db_link->link_next_pointer, connection);

        if (link->_db_ptr == db_header->link_last)
            db_header->link_last = db_link->link_prev_pointer;

        database_free_blocks((pointer[LINK_DELETE_BLOCKS_NUM]) {
                link->_db_ptr,
                db_link->link_name_string_pointer,
        }, LINK_DELETE_BLOCKS_NUM, connection);
    }

    database_update_header(connection);
    db_link_free(db_link);

    return rc;
}

RC link_update(
        LinkVector *link_vector,
        String new_name,
        LinkType new_link_type,
        Connection *connection
) {
    RC rc = RC_OK;

    Link *link;
    DB_Link *db_link = db_link_init();

    V_ITERATE(link_vector, i) {
        link = link_vector_get(i, link_vector);

        connection_move_to_pointer(link->_db_ptr, connection);
        db_link_retrieve(db_link, connection);

        bool changed = false;
        if (string_string_cmp(&new_name, link->name) != 0) {
            pointer string_insertion_pointer;
            rc = string_insert(&new_name, &string_insertion_pointer, connection);
            throw_if_not_ok(rc);

            database_free_blocks((pointer[1]) {db_link->link_name_string_pointer}, 1, connection);
            db_link->link_name_string_pointer = string_insertion_pointer;
            changed = true;
        }

        if (db_link->link_type != new_link_type) {
            db_link->link_type = new_link_type;
            changed = true;
        }

        if (changed) {
            connection_move_to_pointer(link->_db_ptr, connection);
            rc = db_link_update(db_link, connection);
            throw_if_not_ok(rc);
        }
    }

    database_update_header(connection);
    db_link_free(db_link);

    return rc;
}
