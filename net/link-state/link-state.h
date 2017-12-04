/**

    @file   link-state.h
    @author Rhys Thomas <rt8g15@soton.ac.uk>
    @date   2017-12-03T13:42

    @brief  Functions and types for link-state routing.

    Cost metric is hops. Default hop is 1 since nodes are added to the LS
    table if an LSA is received (i.e. the sender is in range for one hop).

*/

#ifndef LINK_STATE_H
#define LINK_STATE_H

#define MAX_TABLE_SIZE 7 ///< Maximum number of nodes in a table.
static uint32_t seq; ///< Global sequence number, increments for each LSP.

#include "net.h"

// TODO static array for link-state table. No malloc.

// Link-state element.
typedef struct {

} ls_elem_t;

/// Link-state packet.
typedef struct {
    uint8_t src_addr;
    uint32_t seq;
    uint16_t age;
    ls_elem_t table[MAX_TABLE_SIZE];
} lsp_t;

/// Table of tables, with
ls_elem_t table[MAX_TABLE_SIZE][MAX_TABLE_SIZE]; /*
something like this?
= {
    0xAA, {
        {0xAB, 1},
        {0xAC, 1},
        {0xBA, 2}
    },
    0xAB, {
        {0xAB, 1},
        {0xAC, 1},
        {0xBA, 2}
    },
}
*/

/**
    @brief  Calculate shortest route from src to dest.

    @param  src : Source node.
    @param  dest : Destination node.
    @param[out] path : Returned node path from src to dest.
    @see    Tanenbaum 5th ed., figure 5-8, p. 369.
*/
void shortest_path(uint8_t src, uint8_t dest, uint8_t path[]);

#endif // LINK_STATE_H
