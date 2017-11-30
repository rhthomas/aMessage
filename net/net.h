/**

    @file   net.h
    @author Rhys Thomas <rt8g15@soton.ac.uk>
    @date   2017-11-26

    @brief  NET layer function prototypes.

*/

#ifndef NET_H
#define NET_H

#include <stdint.h>
#include "errors.h" // has error_t
#include "cksum.h"
#include "net-packets.h"

#ifndef NULL
#define NULL (void *)0
#endif // NULL

#define LOCAL_ADDRESS 0xAA
#define VERSION       0 // change this dependant on what version is running

/// NET packet content type.
typedef enum {
    LSA, ///< Link state advertisement.
    APP, ///< Application data from TRAN layer.
    LSP  ///< Link state packet with table data.
} net_type_t;

/// NET packet structure
typedef union {
    struct { // __attribute__((__packed__)) {
        // ctrl byte (LSb first)
        uint8_t    vers : 2;
        uint8_t    hop  : 3;
        net_type_t type : 2;
        uint8_t    ack  : 1;
        uint8_t    res;
        // ctrl byte
        uint8_t    src_addr;
        uint8_t    dest_addr;
        uint8_t    length;
        uint8_t   *tran;
        uint16_t   cksum;
    };
    unsigned char elem[0];
} net_packet_t;


//---------- public methods ----------//

/**
    @brief  Add data to the TX buffer.

    Called by TRAN layer or NET layer?

    @param  data : Byte array being added to buffer.
    @param  length : Length of byte array.
    @param  addr : Address of sender (current node).
    @retval ERROR_OK : No errors.
    @retval ERROR_NET_NOBUFS : NET buffer is full.
*/
error_t net_tx(uint8_t *data, uint8_t length, uint8_t addr);

/**
    @brief  Add data to the RX buffer.

    Called by DLL layer.

    @param  data : Byte array being added to buffer.
    @param  length : Length of byte array.
    @param  addr : Address of sender (current node).
    @retval ERROR_OK : No errors.
    @retval ERROR_NET_NOBUFS : NET buffer is full.
*/
error_t net_rx(uint8_t *data, uint8_t length, uint8_t addr);

/**
    @brief  Statemachine for NET layer operation.

    Called by main clock tick interrupt.

    @see Physical [PHY] layer.
*/
void net_tick(void);


//---------- internal functions ----------//

/**
    @brief  Handles the transmission of packets.

    Mainly passing APP packets from TRAN down to DLL. Routing will be done
    here?

    @param  p : Packet to be sent.
    @retval ERROR_OK : No errors.
    @retval ERROR_DLL_NOBUFS : DLL buffer is full.
*/
error_t net_tx_handler(net_packet_t p);

/**
    @brief  Handles receiving packets.

    @param  p : Packet to handle.
    @return error_t
*/
error_t net_rx_handler(net_packet_t p);

/**
    @brief  Queues up an LSA to transmit.

    @retval ERROR_OK : No errors.
    @retval ERROR_DLL_NOBUFS : DLL buffer is full.
*/
error_t net_send_lsa(void)


//---------- utility functions ----------//

/**
    @brief  Convert NET packet to byte array.
*/
uint8_t *net_to_array(net_packet_t *p);

/**
    @brief  Convert byte array to NET packet.
*/
net_packet_t net_to_struct(uint8_t *data, uint8_t length);


//---------- buffers ----------//

/// Byte string buffer for passing arrays around.
typedef struct {
    uint8_t *data;   ///< Data array.
    uint8_t  length; ///< Length of array.
} bytestring_t;

/// Up-stack buffer.
bytestring_t net_tx_buffer;

/// Down-stack buffer.
bytestring_t net_rx_buffer;


//---------- states ----------//

/// State machine states.
typedef enum {
    TX,
    RX,
} state_t;

// memory for present/next states
state_t pres_s, next_s;


#endif // NET_H
