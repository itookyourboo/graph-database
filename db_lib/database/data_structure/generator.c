#include <malloc.h>
#include "generator.h"


void *generator_next(Generator *generator) {
    void *result = generator->next_function(generator);
    generator->result = result;
    return generator->result;
}

Generator *generator_init() {
    Generator *generator = malloc(sizeof(Generator));
    generator->result = NULL;
    return generator;
}

void generator_free(Generator *generator) {
    free(generator);
}
