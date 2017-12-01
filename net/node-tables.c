#include "node-tables.h"

void new_node(node_t *list, node_t add, uint8_t *num_items)
{
    if (*num_items >= 0 && *num_items < MAX_NODES) {
        array[*num_items] = value;
        *num_items += 1;
    }
}

void update_node_table(node_t *list, uint8_t *num_items)
{
    uint8_t i = 0;
    // go through array and -- age
    while (i < *num_items) {
        array[i].age--;
        if (array[i].age == 0) {
            // remove node
            for (uint8_t j=i; j<*num_items-1; j++) {
                array[j] = array[j+1];
            }
            *num_items -= 1;
            continue;
        }
        i++;
    }
}

int8_t search_list(node_t *list, uint8_t search, uint8_t *num_items)
{
    if (*num_items) {
        for (int i=0; i<*num_items; i++) {
            if (search == list[i].addr) {
                return i;
            }
        }
    }
    return -1;
}
