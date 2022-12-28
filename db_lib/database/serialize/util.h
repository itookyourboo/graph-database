#ifndef LOW_LEVEL_PROGRAMMING_TASK1_CORE_H
#define LOW_LEVEL_PROGRAMMING_TASK1_CORE_H

#include <string.h>
#include <malloc.h>
#include "../connection.h"
#include "block_serialize.h"

#define field_to_bytes(struct_, field, bytes, ptr) do {                 \
memcpy(&bytes[ptr], &struct_->field, sizeof(struct_->field));           \
ptr += sizeof(struct_->field);                                          \
} while(0)

#define field_array_to_bytes(struct_, field, size, bytes, ptr) do {     \
memcpy(&bytes[ptr], struct_->field, sizeof(*struct_->field) * size);                              \
ptr += sizeof(*struct_->field) * size;                                   \
} while(0)

#define field_from_bytes(struct_, field, bytes, ptr) do {               \
memcpy(&struct_->field, &bytes[ptr], sizeof(struct_->field));           \
ptr += sizeof(struct_->field);                                          \
} while(0);

#define field_array_from_bytes(struct_, field, size, bytes, ptr) do {   \
struct_->field = malloc(size * sizeof(*struct_->field));                \
memcpy(struct_->field, &bytes[ptr], size * sizeof(*struct_->field));    \
ptr += sizeof(*struct_->field) * size;                                  \
} while(0)

#define write_field(struct_, field, fp)                                 \
fwrite(&struct_->field, sizeof(struct_->field), 1, fp)
#define read_field(struct_, field, fp)                                  \
fread(&struct_->field, sizeof(struct_->field), 1, fp)
#define write_array(struct_, size, field, fp)                           \
fwrite(struct_->field, sizeof(*struct_->field), size, fp)
#define read_array(struct_, size, field, fp)                            \
struct_->field = malloc(size * sizeof(*struct_->field));                \
fread(struct_->field, sizeof(*struct_->field), size, fp)

#define write_array_zeros(size, fp)                                     \
fwrite(&(byte){0}, 1, size, fp)


RC change_prev_pointer(pointer object_pointer, pointer prev_pointer, Connection *connection);
RC change_next_pointer(pointer object_pointer, pointer next_pointer, Connection *connection);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_CORE_H
