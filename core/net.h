/**

    @file   net.h
    @author Rhys Thomas <rt8g15@soton.ac.uk>
    @date   2017-11-26

    @brief  NET layer function prototypes.

*/

#ifndef NET_H
#define NET_H

#include <stdint.h>

#include "errors.h"
#include "node-tables.h"
// these are down the bottom to stop linker errors
// #include "link-state.h"
// #include "cksum.h"
// #include "net-packets.h"

#ifndef NULL
#define NULL (void *)0
#endif // NULL

#define LOCAL_ADDRESS 0xAA
// #ifdef DEBUG
//     #define LOCAL_ADDRESS 0xAA
// #else
//     #define MAC_EEPROM_LOC E2END
//     #define LOCAL_ADDRESS  eeprom_read_byte(MAC_EEPROM_LOC)
// #endif // DEBUG

#define VERSION 1 ///< Service version.

/// NET packet content type.
typedef enum {
    LSA, ///< Link state advertisement.
    APP, ///< Application data from TRAN layer.
    LSP  ///< Link state packet with table data.
} net_type_t;

/// NET packet structure
typedef union {
    struct __attribute__((__packed__)) {
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
        uint8_t    tran[121];
        uint16_t   cksum;
    };
    unsigned char elem[0];
} net_packet_t;

//---------- public methods ----------//

/**
    @brief  Add data to the TX buffer.

    Called by TRAN layer.

    @param  data : Byte array being added to buffer.
    @param  length : Length of byte array.
    @param  mac : Address of destination node.
    @retval ERROR_OK : No errors.
    @retval ERROR_NET_NOBUFS : NET buffer is full.
*/
error_t net_tx(uint8_t *data, uint8_t length, uint8_t mac);

/**
    @brief  Add data to the RX buffer.

    Called by DLL layer.

    @param  data : Byte array being added to buffer.
    @param  length : Length of byte array.
    @param  mac : Address of sending node.
    @retval ERROR_OK : No errors.
    @retval ERROR_NET_NOBUFS : NET buffer is full.
*/
error_t net_rx(uint8_t *data, uint8_t length, uint8_t mac);

/**
    @brief  Function calls for NET layer operation.

    Called by main clock tick interrupt.

    @see    Physical [PHY] layer.
*/
void net_tick(void);

//---------- internal functions ----------//

/**
    @brief  Handles the transmission of packets.

    1. Check the TX buffer has stuff in.
    2. Pop TRAN data from buffer and pad with NET fields.
    3. Pass down to DLL for transmission.

    @retval ERROR_OK : No errors or the buffer is empty.
    @retval ERROR_DLL_NOBUFS : DLL buffer is full.
*/
error_t net_tx_handler(void);

/**
    @brief  Handles packets in RX buffer.

    1. Check the RX buffer has stuff in.
    2. Pop data from buffer and convert to structure.
    3. Validate checksum, return ERROR_INV_CKSUM if they don't match.
    4. Check message is addressed to this node.
    5. Process message.

    @return error_t
    @retval ERROR_OK : If no errors or the buffer is empty.
*/
error_t net_rx_handler(void);

/**
    @brief  Queues up an LSA to transmit.

    Call when the node comes up, and every 10ms just to remind remote nodes that
    this node is still alive.

    @retval ERROR_OK : No errors.
    @retval ERROR_DLL_NOBUFS : DLL buffer is full.
*/
error_t net_send_lsa(void);

//---------- utility functions ----------//

/**
    @brief  Convert NET packet to byte array.

    @param  p : Packet to convert to array.
*/
uint8_t *net_to_array(net_packet_t *p);

/**
    @brief  Convert byte array to NET packet.

    @param  data : Byte array to convert to structure.
    @param  length : Length of data.
*/
net_packet_t net_to_struct(uint8_t *data, uint8_t length);

//---------- buffers ----------//

/// Byte string buffer for passing arrays around.
typedef struct {
    uint8_t data[128]; ///< Data array.
    uint8_t length;    ///< Length of array.
    uint8_t mac;       ///< MAC address.
} bytestring_t;

#define MAX_BUFFER_SIZE 3

/// Buffer type for NET layer
typedef struct {
    bytestring_t buffer[MAX_BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
} net_buffer_t;

net_buffer_t net_tx_buffer; ///< TX buffer.
net_buffer_t net_rx_buffer; ///< RX buffer.

/**
    @brief  Push bytestring to buffer.

    @param  buf : Buffer to add to.
    @param  bs : Bytestring to add to buffer.
    @see    net_tx_buffer
    @see    net_rx_buffer
*/
error_t net_buffer_push(net_buffer_t *buf, bytestring_t bs);

/**
    @brief  Pop data from buffer.

    @param  buf : Buffer to pop from.
    @param  out_bs : Bytestring to pop the buffer to.
    @see    net_tx_buffer
    @see    net_rx_buffer
*/
error_t net_buffer_pop(net_buffer_t *buf, bytestring_t *out_bs);

/**
    @brief  Get oldest data in buffer without popping.

    @param  buf : Buffer to pop from.
    @param  out_bs : Bytestring to pop the buffer to.
    @see    net_tx_buffer
    @see    net_rx_buffer
*/
error_t net_buffer_peek(net_buffer_t *buf, bytestring_t *out_bs);

/**
    @brief  Get the size of the buffer.

    @return Size of buffer.
*/
uint8_t net_buffer_size(net_buffer_t *buf);

//---------- temp functions ----------//

error_t dll_tx(uint8_t *data, uint8_t length, uint8_t dest);
error_t tran_rx(uint8_t *data, uint8_t length, uint8_t src);

//---------- extra includes that fail up top ----------//

#include "cksum.h"
#include "net-packets.h"
#include "link-state.h"

// #include "transport.h"

#endif // NET_H
