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

#ifndef NULL
#define NULL (void *)0
#endif // NULL

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

    Called by TRAN layer.

    @param  b : Bytestring to add to buffer.
    @retval ERROR_OK : No errors.
    @retval ERROR_NET_NOBUFS : NET buffer is full.
*/
error_t net_tx(uint8_t *data, uint8_t length, uint8_t addr);

/**
    @brief  Add data to the RX buffer.

    Called by DLL layer.

    @param  b : Bytestring to add to buffer.
    @retval ERROR_OK : No errors.
    @retval ERROR_NET_NOBUFS : NET buffer is full.
*/
error_t net_rx(uint8_t *data, uint8_t length, uint8_t addr);

/**
    @brief  Add data to the RX buffer.

    Called by main clock tick interrupt.

    @param  b : Bytestring to add to buffer.
    @retval ERROR_OK : No errors.
    @retval ERROR_NET_NOBUFS : NET buffer is full.
*/
void net_tick(void);


//---------- internal functions ----------//

/**
    @brief  Creates and sends broadcast NET packet to DLL.
*/
error_t net_send_broadcast();


//---------- utility functions ----------//

/**
    @brief  Convert NET packet to byte array.
*/
uint8_t *net_to_array(net_packet_t *p);

/**
    @brief  Convert byte array to NET packet.
*/
net_packet_t net_to_struct(uint8_t *data, uint8_t length);

/**
    @brief  Handles receiving packets.

    @param  p : Packet to handle.
*/
void net_rx_handler(net_packet_t p);


//---------- buffers ----------//

typedef struct {
    uint8_t *data;
    uint8_t  length;
} bytestring_t;

bytestring_t net_tx_buffer;
bytestring_t net_rx_buffer;


//---------- states ----------//

typedef enum {
    IDLE,
    TX,
    RX,
    OTHER
} state_t;

// present/next states
state_t pres_s, next_s;


#endif // NET_H
