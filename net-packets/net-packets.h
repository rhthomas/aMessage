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

#ifndef VERSION
#define VERSION 0
#endif // VERSION

/**
    @brief  Create acknoledgement packets.

    @param  src : Address of sender.
    @param  dest : Address of intended receiver.
    @return A NET packet.
*/
net_packet_t net_ack_packet(uint8_t src, uint8_t dest);

/**
    @brief  Create LSA packet for echo message.

    @param  src : Address of sender.
    @return A NET packet.
*/
net_packet_t net_lsa_packet(uint8_t src);

/**
    @brief  Create broadcast NET packet.

    @param  src : Address of sender.
    @param  data : Byte array of data to send.
    @param  data_len : Length of data array.
    @return A NET packet.
*/
net_packet_t net_bcast_packet(uint8_t src, uint8_t *data, uint8_t data_len);

/**
    @brief  Pad TRAN data to form a NET data packet.

    @param  src : Address of sender.
    @param  dest : Intended recipient.
    @param  data : Byte array of TRAN data to pad.
    @param  data_len : Lenght of TRAN data.
    @return A net_packet_t object.
*/
net_packet_t net_data_packet(uint8_t src, uint8_t dest, uint8_t *data,
    uint8_t data_len);

#endif // NET_PACKETS_H
