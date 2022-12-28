#include <malloc.h>
#include <string.h>
#include "database_model.h"


DB_Header *db_header_init() {
    DB_Header *db_header = malloc(sizeof(DB_Header));
    memset(db_header, nullptr, sizeof(DB_Header));
    return db_header;
}

amount db_header_size(DB_Header *db_header) {
    return sizeof(DB_Header);
}

void db_header_free(DB_Header *db_header) {
    free(db_header);
}
