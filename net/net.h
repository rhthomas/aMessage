#ifndef NET_H
#define NET_H

#include "errors.h" /* has error_t */
#include "bytestring.h" /* has bytestring type */
#include "buffer.h" /* harry's buffer code */

typedef enum {
    LSA,
    APP,
    LSP
} net_type_t;

typedef struct {
    /* ctrl byte (LSb first) */
    uint8_t    vers : 2;
    uint8_t    hop  : 3;
    net_type_t type : 2;
    uint8_t    ack  : 1;
    uint8_t    res;
    /* ctrl byte */
    uint8_t    src_addr;
    uint8_t    dest_addr;
    uint8_t    length;
    uint8_t*   tran;
    uint16_t   csum;
} net_packet_t;

/* public methods */
error_t net_tx(char* data, char length);
error_t net_rx(char* data, char length);
void net_tick();

/* internal functions */
bytestring net_to_array( net_packet_t p );
net_packet_t net_to_struct( char* data );

/* buffers */
buffer_t *net_tx_buffer;
buffer_t *net_rx_buffer;

#endif /* NET_H */
