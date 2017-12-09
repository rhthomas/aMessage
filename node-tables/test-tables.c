/**

    Testing static node list functions.

*/

#include <stdio.h>
#include <stdint.h>

#include "node-tables.h"

void print(void)
{
    for (uint8_t i=0; i<MAX_NODES; i++) {
        if (known_nodes[i].occupied) {
            printf("{0x%02x, %d}, ",
                known_nodes[i].node.addr, known_nodes[i].node.age);
        } else {
            printf("{____, _}, ");
        }
    }
    printf("\n");
}

int main(void)
{
    /*--------------------------------------------------------------------------
    TEST: new_node

    Stress test adding too many nodes to the list. It should simply return early
    if there is no room in the array.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("----------\ntest add too many\n----------\n");
    for (int i=0; i<15; i++) {
        printf("[ list ]\t");
        new_node(
            (node_t){i,(i%3)+1}
        );
        print();
    }
    printf("\n");

    /*--------------------------------------------------------------------------
    TEST: search_list

    Test searching through the list for node 0x03. This function returns the
    index of the node in the array.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("----------\ntest searching\n----------\n");
    printf("[search]\t");
    int8_t index = search_list(0x03);
    if (index == -1) {
        printf("not in list\n");
    } else {
        printf("0x%02x is at index %d\n", 0x03, index);
    }
    printf("\n");

    /*--------------------------------------------------------------------------
    TEST: update_node_table

    Check that the function correctly iterates through the table, deleting nodes
    if their age == 0.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("----------\ntest updating\n----------\n");
    for (int i=0; i<0xF; i++) {
        printf("[update]\t");
        update_node_table();
        print();
    }
    printf("\n");

    /*--------------------------------------------------------------------------
    TEST: search_list

    Test that if searching for an unknown node, the function returns -1.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("----------\ntest missing\n----------\n");
    printf("[search]\t");
    index = search_list(0x03);
    if (index == -1) {
        printf("not in list\n");
    } else {
        printf("0x%02x is at index %d\n", 0x03, index);
    }

    return 0;
}
