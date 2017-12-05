#include "node-tables.h"

uint8_t num_nodes = 0;

void new_node(node_t add)
{
    if (num_nodes >= 0 && num_nodes < MAX_NODES) {
        known_nodes[num_nodes] = add;
        num_nodes += 1;
    }
}

void update_node_table()
{
    uint8_t i = 0;
    // go through array and -- age
    while (i < num_nodes) {
        known_nodes[i].age--;
        if (known_nodes[i].age == 0) {
            // remove node
            for (uint8_t j=i; j<num_nodes-1; j++) {
                known_nodes[j] = known_nodes[j+1];
            }
            num_nodes -= 1;
            continue;
        }
        i++;
    }
}

int8_t search_list(uint8_t search)
{
    if (num_nodes) {
        for (uint8_t i=0; i<num_nodes; i++) {
            if (search == known_nodes[i].addr) {
                return i;
            }
        }
    }
    return -1;
}
