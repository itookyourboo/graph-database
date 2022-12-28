#ifndef LOW_LEVEL_PROGRAMMING_TASK1_STRING_MODEL_H
#define LOW_LEVEL_PROGRAMMING_TASK1_STRING_MODEL_H

#include <stdbool.h>
#include "../mem.h"

typedef struct __attribute__((packed)) {
    amount size;
    char *content;
} String;

String *string_init();
amount string_size(String *string);
void string_free(String *string);

int string_string_cmp(String *first, String *second);
int string_char_cmp(String *first, char *second);

String *string_copy(String *src);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_STRING_MODEL_H
