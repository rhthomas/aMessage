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
#include "node-tables.h"
// #include "cksum.h"
// #include "net-packets.h"

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
    struct { //__attribute__((__packed__)) {
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
    @param  mac : Address of node.
    @retval ERROR_OK : No errors.
    @retval ERROR_NET_NOBUFS : NET buffer is full.
*/
error_t net_tx(uint8_t *data, uint8_t length, uint8_t mac);

/**
    @brief  Add data to the RX buffer.

    Called by DLL layer.

    @param  data : Byte array being added to buffer.
    @param  length : Length of byte array.
    @param  mac : Address of node.
    @retval ERROR_OK : No errors.
    @retval ERROR_NET_NOBUFS : NET buffer is full.
*/
error_t net_rx(uint8_t *data, uint8_t length, uint8_t mac);

/**
    @brief  Statemachine for NET layer operation.

    Called by main clock tick interrupt.

    @see Physical [PHY] layer.
*/
void net_tick(void);

//---------- internal functions ----------//

/**
    @brief  Handles the transmission of packets.

    1. Check the TX buffer has stuff in.
    2. Pop TRAN data from buffer and pad with NET fields.
    3. Pass down to DLL for transmission.

    @retval ERROR_OK : No errors.
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
*/
uint8_t *net_to_array(net_packet_t *p);

/**
    @brief  Convert byte array to NET packet.
*/
net_packet_t net_to_struct(uint8_t *data, uint8_t length);

//---------- buffers ----------//

/// Byte string buffer for passing arrays around.
typedef struct {
    uint8_t data[121]; ///< Data array.
    uint8_t length;    ///< Length of array.
    uint8_t mac;       ///< MAC address.
} bytestring_t;

#define MAX_BUFFER_SIZE 2 ///< Don't really need two buffers at NET layer.

// bytestring_t net_tx_buffer[MAX_BUFFER_SIZE];
// extern uint8_t net_tx_size;
//
// bytestring_t net_rx_buffer[MAX_BUFFER_SIZE];
// extern uint8_t net_rx_size;

typedef struct {
    bytestring_t buffer[MAX_BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
} net_buffer_t;

net_buffer_t net_tx_buffer;
net_buffer_t net_rx_buffer;

/**
    @brief  Returns first element in buffer.
*/
// error_t net_buffer_push(bytestring_t *buffer, uint8_t *size, uint8_t *data,
//     uint8_t length, uint8_t mac);
error_t net_buffer_push(net_buffer_t *buf, bytestring_t bs);

/**
    @brief  Returns first element in buffer.
*/
// bytestring_t net_buffer_pop(bytestring_t *buffer, uint8_t *size);
error_t net_buffer_pop(net_buffer_t *buf, bytestring_t *out_bs);

/**
    @brief  Return oldest data without incrementing the tail.
*/
error_t net_buffer_peak(net_buffer_t *buf, bytestring_t *out_bs);

/**
    @brief  Get the size of the buffer.

    @return Size of buffer.
*/
uint8_t net_buffer_size(net_buffer_t *buf);

//---------- states ----------//

/// State machine states.
typedef enum {
    TX,
    RX,
} state_t;

// memory for present/next states
state_t pres_s, next_s;

//---------- temp functions ----------//

error_t dll_tx(uint8_t *data, uint8_t length, uint8_t dest);
error_t tran_rx(uint8_t *data, uint8_t length, uint8_t src);


// TODO refactor code
#include "cksum.h"
#include "net-packets.h"

#endif // NET_H
