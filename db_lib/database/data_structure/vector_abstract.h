#ifndef LOW_LEVEL_PROGRAMMING_TASK1_VECTOR_ABSTRACT_H
#define LOW_LEVEL_PROGRAMMING_TASK1_VECTOR_ABSTRACT_H

#include "generator.h"

#define INITIAL_CAPACITY 256

#define DECLARE_VECTOR(Type, type)                                                                                      \
struct Type##Vector;                                                                                                    \
typedef struct Type##Vector Type##Vector;                                                                               \
Type##Vector *type##_vector_init();                                                                                     \
void type##_vector_free(Type##Vector *type##_vector);                                                                   \
void type##_vector_push(Type *type, Type##Vector *type##_vector);                                                       \
Type *type##_vector_get(size_t index, Type##Vector *type##_vector);                                                     \
Type##Vector *type##_vector_fetch_all(Generator *generator);                                                            \
size_t type##_vector_get_size(Type##Vector *type##_vector);                                                             \
size_t type##_vector_get_capacity(Type##Vector *type##_vector);                                                         \
void type##_vector_set(size_t index, Type *type, Type##Vector *type##_vector);                                          \
Type *type##_vector_pop(Type##Vector *type##_vector);                                                                   \

#define DEFINE_VECTOR(Type, type)                                                                                       \
struct Type##Vector {                                                                                                   \
    Type **array;                                                                                                       \
    size_t size;                                                                                                        \
    size_t capacity;                                                                                                    \
};                                                                                                                      \
                                                                                                                        \
Type##Vector *type##_vector_init() {                                                                                    \
    Type##Vector *type##_vector = malloc(sizeof(Type##Vector));                                                         \
    type##_vector->capacity = INITIAL_CAPACITY;                                                                         \
    type##_vector->size = 0;                                                                                            \
    type##_vector->array = malloc(type##_vector->capacity * sizeof(*type##_vector->array));                             \
                                                                                                                        \
    return type##_vector;                                                                                               \
}                                                                                                                       \
                                                                                                                        \
void type##_vector_free(Type##Vector *type##_vector) {\
    for (size_t i = 0; i < type##_vector->size; type##_free(type##_vector->array[i++]));                                \
    free(type##_vector->array);                                                                                         \
    free(type##_vector);                                                                                                \
}                                                                                                                       \
                                                                                                                        \
void type##_vector_push(Type *item, Type##Vector *type##_vector) {                                                      \
    if (type##_vector->size == type##_vector->capacity) {                                                               \
        type##_vector->capacity *= 2;                                                                                   \
        type##_vector->array = realloc(type##_vector->array, type##_vector->capacity * sizeof(*type##_vector->array));  \
    }                                                                                                                   \
    type##_vector->array[type##_vector->size++] = item;                                                                 \
}                                                                                                                       \
                                                                                                                        \
Type *type##_vector_get(size_t index, Type##Vector *type##_vector) {                                                    \
    return type##_vector->array[index];                                                                                 \
}                                                                                                                       \
                                                                                                                        \
Type##Vector *type##_vector_fetch_all(Generator *generator) {                                                           \
    Type##Vector *type##_vector = type##_vector_init();                                                                 \
    Type *item, *item_to_add;                                                                                           \
    while ((item = generator_next(generator)) != NULL) {                                                                \
        item_to_add = type##_copy(item);                                                                                \
        type##_vector_push(item_to_add, type##_vector);                                                                 \
    }                                                                                                                   \
    return type##_vector;                                                                                               \
}                                                                                                                       \
                                                                                                                        \
size_t type##_vector_get_size(Type##Vector *type##_vector){                                                             \
    return type##_vector->size;                                                                                         \
}                                                                                                                       \
                                                                                                                        \
size_t type##_vector_get_capacity(Type##Vector *type##_vector) {                                                        \
    return type##_vector->capacity;                                                                                     \
}                                                                                                                       \
                                                                                                                        \
void type##_vector_set(size_t index, Type *type, Type##Vector *type##_vector) {                                         \
    type##_vector->array[index] = type;                                                                                 \
}                                                                                                                       \
                                                                                                                        \
Type *type##_vector_pop(Type##Vector *type##_vector) {                                                                  \
    return type##_vector->array[--type##_vector->size];                                                                 \
}                                                                                                                       \

#endif //LOW_LEVEL_PROGRAMMING_TASK1_VECTOR_ABSTRACT_H
