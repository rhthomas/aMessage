#include "node-tables.h"

void new_node(node_t add)
{
    for (uint8_t i=0; i<MAX_NODES; i++) {
        if (!(known_nodes[i].occupied)) {
            known_nodes[i].occupied = 1;
            known_nodes[i].node = add;
            return;
        }
    }
}

void update_node_table()
{
    for (uint8_t i=0; i<MAX_NODES; i++) {
        if (known_nodes[i].occupied) {
            if (--known_nodes[i].node.age == 0) {
                // node dead, free memory
                known_nodes[i].occupied = 0;
            }
        }
    }
}

int8_t search_list(uint8_t search)
{
    for (uint8_t i=0; i<MAX_NODES; i++) {
        if (known_nodes[i].occupied) {
            if (search == known_nodes[i].node.addr) {
                return i;
            }
        }
    }
    return -1;
}
