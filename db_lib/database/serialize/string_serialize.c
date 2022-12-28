#include <string.h>
#include <malloc.h>

#include "util.h"
#include "string_serialize.h"

RC string_to_bytes(String *string, byte *bytes) {
    amount i = 0;
    field_to_bytes(string, size, bytes, i);
    field_array_to_bytes(string, content, string->size, bytes, i);
    return RC_OK;
}

RC string_from_bytes(String *string, byte *bytes) {
    amount i = 0;
    field_from_bytes(string, size, bytes, i);
    field_array_from_bytes(string, content, string->size, bytes, i);
    return RC_OK;
}
