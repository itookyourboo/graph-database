#include <malloc.h>
#include <string.h>
#include "string_model.h"

String *string_init() {
    String *string = malloc(sizeof(String));
    string->content = NULL;
    return string;
}

amount string_size(String *string) {
    return (
            sizeof(string->size) +
            string->size * sizeof(*string->content)
    );
}

void string_free(String *string) {
    free(string);
}

#define MN(a, b) (a) < (b) ? (a) : (b)

int string_string_cmp(String *first, String *second) {
    return strncmp(first->content, second->content, MN(first->size, second->size));
}

int string_char_cmp(String *first, char *second) {
    return strncmp(first->content, second, MN(first->size, sizeof(second)));
}

String *string_copy(String *src) {
    String *string = string_init();
    string->size = src->size;

    size_t size = string->size * sizeof(*string->content);
    string->content = malloc(size);
    memcpy(string->content, src->content, size);

    return string;
}
