#ifndef LOW_LEVEL_PROGRAMMING_TASK1_STRING_SERIALIZE_H
#define LOW_LEVEL_PROGRAMMING_TASK1_STRING_SERIALIZE_H

#include "../exceptions.h"
#include "../model/string_model.h"
#include "../mem.h"

RC string_to_bytes(String *string, byte *bytes);
RC string_from_bytes(String *string, byte *bytes);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_STRING_SERIALIZE_H
