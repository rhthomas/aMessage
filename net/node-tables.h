/**

    @file   node-tables.h
    @author Rhys Thomas <rt8g15@soton.ac.uk>
    @date   2017-12-01

    @brief  Functions for managing node directories.

*/

#ifndef NODE_TABLES_H
#define NODE_TABLES_H

#include <stdint.h>

#define MAX_NODES 7
#define START_AGE 5000 // 1ms function call period = 5s life

typedef struct {
    uint8_t  addr;
    uint16_t age;
} node_t;

node_t known_nodes[MAX_NODES];
uint8_t num_nodes = 0; // keeps track of static array size

/**
    @brief  Add new nodes to the list of known nodes.

    @param  list : Pointer to list of nodes.
    @param  add : Node to add.
    @param  num_items : Pointer to array size tracker.
    @see    list-test.c example
*/
void new_node(node_t *list, node_t add, uint8_t *num_items);

/**
    @brief  Updates all nodes in table.

    Decrements age of each node on call. Any nodes with an age of 0 will be
    deleted from the list.

    @param  list : Pointer to list of nodes.
    @param  num_items : Pointer to array size tracker.
    @see    list-test.c example
*/
void update_node_table(node_t *list, uint8_t *num_items);

/**
    @brief  Search for node in list.

    @param  list : Pointer to list of nodes.
    @param  search : Address of node to look for.
    @param  num_items : Pointer to array size tracker.
    @return Index of node in the list.
    @retval -1 : Node does not exist.
*/
int8_t search_list(node_t *list, uint8_t search, uint8_t *num_items);

#endif // NODE_TABLES_H
