#include <math.h>
#include <stdio.h>

#include "visualize.h"
#include "model/string_model.h"
#include "model/block_model.h"

void string_print(String *string) {
    for (amount i = 0; i < string->size; i++) {
        printf("%c", string->content[i]);
    }
}

void block_node_print(BlockNode *block_node) {
    printf("[(%s); &%04lu; %04d]",
           block_node->block->is_free ? " " : "X",
           block_node->block_pointer,
           block_node->block->capacity);
//           block_size(block_node->block));
}

void block_heap_print(BlockHeap *block_heap) {
    if (block_heap_size(block_heap) == 0) return;

    amount string_size = 18;

    amount size = block_heap_size(block_heap);
    amount levels = (amount) log2((double) size);
    amount length = (amount) pow(2, levels);
    amount node_i = 0;
    for (amount i = 0; i <= levels; i++) {
        amount blocks_on_level = (amount) pow(2, i);
        amount block_offset = string_size / 2 * (length / blocks_on_level - 1);
        for (amount j = 0; j < blocks_on_level; j++) {
            for (amount k = 0; k < block_offset; k++) printf(" ");
            block_node_print(
                    block_node_vector_get(node_i++, block_heap->block_node_vector)
            );
            for (amount k = 0; k < block_offset; k++) printf(" ");

            if (node_i >= size) break;
        }
        printf("\n");
    }
}

void schema_print(Schema *schema) {
    printf("%s\n", schema->name->content);
    for (amount i = 0; i < schema->attr_count; i++)
        printf("- %s (%s)\n",
               schema->attributes[i]->name->content,
               DATA_TYPE_NAMES[schema->attributes[i]->data_type]
        );
}

void node_print(Node *node) {
    printf("(");
    for (amount i = 0; i < node->value_count; i++) {
        value_print(node->values[i]);
        if (i != node->value_count - 1) printf(", ");
    }
    printf(")");
}

void link_print(Link *link) {
    if (link->type == LT_DIRECTED_FROM || link->type == LT_DIRECTED_BOTH)
        printf("<");
    printf("--(");
    string_print(link->name);
    printf(")--");
    if (link->type == LT_DIRECTED_TO || link->type == LT_DIRECTED_BOTH)
        printf(">");
}

void value_print(Value *value) {
    switch (value->data_type) {
        case DT_INTEGER:
            printf("%d", value->integer);
            break;
        case DT_STRING:
            string_print(value->string);
            break;
        case DT_FLOATING:
            printf("%f", value->floating);
            break;
        case DT_BOOLEAN:
            printf(value->boolean ? "true" : "false");
            break;
    }
}

void result_print(Result *result) {
    Return flags = result->return_items;
    printf("[");
    if (flags & RETURN_NODE_FIRST)
        node_print(result->first);
    if (flags & RETURN_LINK)
        link_print(result->link);
    if (flags & RETURN_NODE_SECOND)
        node_print(result->second);
    printf("]");
}