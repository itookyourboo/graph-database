#ifndef LOW_LEVEL_PROGRAMMING_TASK2_PRINTER_H
#define LOW_LEVEL_PROGRAMMING_TASK2_PRINTER_H


#include "query.h"
#include "db_model.h"


void print_query(Query query);
void print_schema(Schema *schema, int depth);


#endif //LOW_LEVEL_PROGRAMMING_TASK2_PRINTER_H
