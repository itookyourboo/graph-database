#ifndef LOW_LEVEL_PROGRAMMING_TASK1_DATA_TYPE_MODEL_H
#define LOW_LEVEL_PROGRAMMING_TASK1_DATA_TYPE_MODEL_H

#include "string_model.h"
#include <stdbool.h>

typedef enum {
    DT_INTEGER = 0,
    DT_STRING = 1,
    DT_FLOATING = 2,
    DT_BOOLEAN = 3
} DataType;

static char *DATA_TYPE_NAMES[] = {
        "INTEGER",
        "STRING",
        "FLOATING",
        "BOOLEAN"
};

typedef int32_t dt_integer;
typedef String *dt_string;
typedef float dt_floating;
typedef bool dt_boolean;

#endif //LOW_LEVEL_PROGRAMMING_TASK1_DATA_TYPE_MODEL_H
