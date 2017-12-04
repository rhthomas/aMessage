/**

    Testing static node list functions.

*/

#include <stdio.h>
#include <stdint.h>

#include "node-tables.h"

uint8_t num_nodes = 0;

void print(node_t *array, uint8_t *num_items)
{
    if (*num_items == 0) {
        printf("empty\n");
        return;
    }
    for (uint8_t i=0; i<*num_items; i++) {
        printf("{0x%02x, %d}, ", array[i].addr, array[i].age);
    }
    printf("\n");
}

int main(void)
{
    printf("test add too many\n");
    for (int i=0; i<15; i++) {
        printf("list..... ");
        new_node(&known_nodes, (node_t){i,(i%3)+1}, &num_nodes);
        print(&known_nodes, &num_nodes);
    }
    printf("\n");

    printf("test searching\n");
    printf("search... ");
    int8_t index = search_list(&known_nodes, 0x03, &num_nodes);
    if (index == -1) {
        printf("not in list\n");
    } else {
        printf("0x%02x is at index %d\n", 0x03, index);
    }
    printf("\n");

    printf("test updating\n");
    for (int i=0; i<0xF; i++) {
        printf("update... ");
        update_node_table(&known_nodes, &num_nodes);
        print(&known_nodes, &num_nodes);
    }
    printf("\n");

    printf("test missing\n");
    printf("search... ");
    index = search_list(&known_nodes, 0x03, &num_nodes);
    if (index == -1) {
        printf("not in list\n");
    } else {
        printf("0x%02x is at index %d\n", 0x03, index);
    }

    return 0;
}
