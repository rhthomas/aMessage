/**

    @file   net-packet.h
    @author Rhys Thomas <rt8g15@soton.ac.uk>
    @date   2017-11-30T11:20

    @brief  Defines packet constructors for NET layer.

*/

#ifndef NET_PACKETS_H
#define NET_PACKETS_H

#include "net.h"
#include "cksum.h"

/**
    @brief  Create acknoledgement packets.

    @param  v : Version of service.
    @param  src : Address of sender.
    @param  dest : Address of intended receiver.
    @return A NET packet.
*/
net_packet_t net_ack_packet(uint8_t v, uint8_t src, uint8_t dest);

/**
    @brief  Create LSA packet for echo message.

    @param  v : Version of service.
    @param  src : Address of sender.
    @return A NET packet.
*/
net_packet_t net_lsa_packet(uint8_t v, uint8_t src);

/**
    @brief  Create LSP packet.

    @param  v : Version of service.
    @param  src : Address of sender.
    @param  dest : Address of intended receiver.
    @param  table : Link state table of source node.
    @return A NET packet.
    @TODO   Not yet implemented.
*/
// net_packet_t net_lsp_packet(uint8_t v, uint8_t src, uint8_t dest,
//     ls_table_t *table);

/**
    @brief  Create broadcast NET packet.

    @param  v : Version of service.
    @param  src : Address of sender.
    @param  data : Byte array of data to send.
    @param  data_len : Length of data array.
    @return A NET packet.
*/
net_packet_t net_bcast_packet(uint8_t v, uint8_t src, uint8_t *data,
    uint8_t data_len)

#endif // NET_PACKETS_H
