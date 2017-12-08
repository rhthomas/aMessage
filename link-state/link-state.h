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

#include "net.h"
#include "node-tables.h"

#define MAX_TABLE_SIZE 7    ///< Maximum number of nodes in a table.
#define UNDEF_LEN      0xFF ///< Large number for undefined hop count.

uint32_t seq; ///< Global sequence number, increments for each LSP.

// 2 bytes
/// Element pairing node address and hops.
typedef struct {
    uint8_t addr; ///< Destination node.
    uint8_t hops; ///< Number of hops to addr
} ls_elem_t;

// 21 bytes
/// Packet for LSP with necessary information.
typedef union {
    struct __attribute__((__packed__)) {
        uint8_t   src; ///< Local node.
        uint32_t  seq; ///< Packet sequence number.
        uint16_t  age; ///< Packet age.
        ls_elem_t connected[MAX_TABLE_SIZE]; ///< List of connected nodes.
    };
    unsigned char elem[0];
} ls_packet_t;

// 22 bytes
/// Local list of all nodes LSPs.
typedef struct {
    uint8_t     occupied; ///< Slot in list is occupied.
    ls_packet_t lsp;      ///< Packet in slot.
} ls_list_t;

/// Local list of each node's link-state table.
ls_list_t ls_list[MAX_TABLE_SIZE];

/**
    @brief  Creates new ls_packet_t from table of elements.

    @param  src : Node who owns the table.
    @param  list : List of nodes connected to src.
    @return Link state packet.
*/
ls_packet_t ls_new_packet(uint8_t src, ls_elem_t *list);

/**
    @brief  Converts incoming array to ls_packet_t.

    @param  array : Link state packet in byte array form.
    @param  length : Length of byte array.
    @return Link state packet.
*/
ls_packet_t array_to_lsp(uint8_t *array, uint8_t length);

/**
    @brief  Create packet containing link-state table.

    @param  src : Address of sender.
    @param  lsp : Link state packet of source node.
    @TODO   Check the sequence number in the list.
    @return A NET packet.
*/
net_packet_t net_lsp_packet(uint8_t src, ls_packet_t *lsp);

/**
    @brief  Add LSP to the ls_list.

    @param  lsp : Packet to add to list.
    @see    ls_list
*/
void add_ls_table(ls_packet_t lsp);

/**
    @brief  Update the link-state table.

    Decrements the age of each node in the list.
*/
void update_ls_table(void);

/**
    @brief  Calculate shortest route from src to dest.

    @param  src : Source node.
    @param  dest : Destination node.
    @param[out] path : Returned node path from src to dest.
    @see    Tanenbaum 5th ed., figure 5-8, p. 369.
    @TODO   write shortest path function
*/
void shortest_path(uint8_t src, uint8_t dest, uint8_t path[]);

#endif // LINK_STATE_H
