#ifndef LOW_LEVEL_PROGRAMMING_TASK1_GENERATOR_H
#define LOW_LEVEL_PROGRAMMING_TASK1_GENERATOR_H


typedef struct Generator {
    void *result;
    void *context;

    void *(*next_function)(struct Generator *);
} Generator;


void *generator_next(Generator *generator);
Generator *generator_init();
void generator_free(Generator *generator);

#endif //LOW_LEVEL_PROGRAMMING_TASK1_GENERATOR_H
