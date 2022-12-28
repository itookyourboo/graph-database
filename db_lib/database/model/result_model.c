#include <string.h>
#include "result_model.h"


Result *result_init() {
    Result *result = malloc(sizeof(Result));
    return result;
}

void result_free(Result *result) {
    node_free(result->first);
    node_free(result->second);
    link_free(result->link);
    free(result);
}

Result *result_copy(Result *src) {
    Result *result = result_init();
    result->return_items = src->return_items;
    if (src->return_items & RETURN_NODE_FIRST)
        result->first = node_copy(src->first);
    if (src->return_items & RETURN_NODE_SECOND)
        result->second = node_copy(src->second);
    if (src->return_items & RETURN_LINK)
        result->link = link_copy(src->link);
    return result;
}

size_t return_count(Return flags) {
    size_t count = 0;
    while (flags > 0) {
        flags &= flags - 1;
        count++;
    }
    return count;
}
