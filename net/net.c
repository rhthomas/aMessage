#include "net.h"

// add data to the TX buffer
error_t net_tx(uint8_t* data, uint8_t length, uint8_t addr)
{
    // push to tx buffer
    // error_t err;
    // if (!err = bs_buffer_push(net_tx_buffer, b)) {
    //     return ERROR_OK;
    // } else {
    //     return err;
    // }

    // error_t err;
    // if (net_rx_buffer != NULL) {
    //     err = ERROR_NET_NOBUFS;
    // } else {
    //     err = ERROR_OK;
    // }
    // // add packet regardless of error
    // net_rx_buffer = {data, length};
    // return err;
    return ERROR_OK;
}

// add data to the RX buffer
error_t net_rx(uint8_t* data, uint8_t length, uint8_t addr)
{
    // push to rx buffer
    // error_t err;
    // if (!err = bs_buffer_push(net_rx_buffer, b)) {
    //     return ERROR_OK;
    // } else {
    //     return err;
    // }

    // error_t err;
    // if (net_rx_buffer != NULL) {
    //     err = ERROR_NET_NOBUFS;
    // } else {
    //     err = ERROR_OK;
    // }
    // // add packet regardless of error
    // net_rx_buffer = {data, length};
    // return err;
    return ERROR_OK;
}

// do stuff with buffers, i.e. if your TX buffer has stuff, pass to DLL.
void net_tick(void)
{
    // // state machine?
    // pres_s = next_s;
    // switch (pres_s) {
    //     case IDLE:
    //         break;
    //     case TX:
    //         // check tx buffer
    //         dll_tx(
    //             net_tx_buffer.data,
    //             net_tx_buffer.length,
    //             net_tx_buffer.data[3] // who to?
    //         );
    //         next_s = IDLE;
    //         break;
    //     case RX:
    //         // check packet is for me
    //         if (net_rx_buffer.data[3] == DEVICE_MAC_ADDR) {
    //             // check rx buffer
    //             tran_rx(
    //                 net_rx_buffer.data,
    //                 net_rx_buffer.length,
    //                 net_tx_buffer.data[2] // who from?
    //             );
    //         }
    //         next_s = IDLE;
    //         break;
    //     default:
    //         break;
    // }
}

uint8_t * net_to_array(net_packet_t *p)
{
    uint8_t size = p->length;
    uint8_t data[size];

    for (int i=0; i<size; i++) {
        data[i] = p->elem[i];
    }
    return data;
}

net_packet_t net_to_struct(uint8_t *data, uint8_t length)
{
    net_packet_t packet;
    for (int i=0; i<length; i++) {
        packet.elem[i] = data[i];
    }
    return packet;
}
