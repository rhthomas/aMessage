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
    printf("test add too many\n");
    for (int i=0; i<15; i++) {
        printf("[ list ]\t");
        new_node(
            (node_t){i,(i%3)+1}
        );
        print();
    }
    printf("\n");

    printf("test searching\n");
    printf("[search]\t");
    int8_t index = search_list(0x03);
    if (index == -1) {
        printf("not in list\n");
    } else {
        printf("0x%02x is at index %d\n", 0x03, index);
    }
    printf("\n");

    printf("test updating\n");
    for (int i=0; i<0xF; i++) {
        printf("[update]\t");
        update_node_table();
        print();
    }
    printf("\n");

    printf("test missing\n");
    printf("[search]\t");
    index = search_list(0x03);
    if (index == -1) {
        printf("not in list\n");
    } else {
        printf("0x%02x is at index %d\n", 0x03, index);
    }

    return 0;
}
