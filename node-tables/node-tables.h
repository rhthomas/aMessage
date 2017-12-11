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
    uint8_t  addr; ///< Address of node.
    uint16_t age;  ///< Age of node LSA. When 0 discard node since its dead.
} node_t;

typedef struct {
    uint8_t occupied; ///< Is this memory address occupied?
    node_t  node;     ///< Address of node in this memory slot.
} node_list_t;

node_list_t known_nodes[MAX_NODES];

/**
    @brief  Add new nodes to the list of known nodes.

    @param  add : Node to add.
    @see    list-test.c example
*/
void new_node(node_t add);

/**
    @brief  Updates all nodes in table.

    Decrements age of each node on call. Any nodes with an age of 0 will be
    deleted from the list.

    @see    list-test.c example
*/
void update_node_table(void);

/**
    @brief  Search for node in list.

    @param  search : Address of node to look for.
    @return Index of node in the list.
    @retval -1 : Node does not exist.
*/
int8_t search_list(uint8_t search);

#endif // NODE_TABLES_H
