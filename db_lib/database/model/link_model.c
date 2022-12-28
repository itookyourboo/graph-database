#include <malloc.h>
#include <string.h>
#include "link_model.h"

DB_Link *db_link_init() {
    DB_Link *db_link = malloc(sizeof(DB_Link));
    return db_link;
}

amount db_link_size(DB_Link *db_link) {
    return sizeof(DB_Link);
}

void db_link_free(DB_Link *db_link) {
    free(db_link);
}

Link *link_init() {
    Link *link = malloc(sizeof(Link));
    return link;
}

amount link_size(Link *link) {
    return (
            string_size(link->name) +
            sizeof(link->type)
    );
}

void link_free(Link *link) {
    string_free(link->name);
    free(link);
}

Link *link_copy(Link *src) {
    Link *link = link_init();
    memcpy(link, src, sizeof(Link));
    link->name = string_copy(src->name);
    return link;
}