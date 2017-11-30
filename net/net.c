/**

    Source file for NET layer.

*/

#include "net.h"

//---------- public methods ----------//

// add data to the TX buffer
error_t net_tx(uint8_t* data, uint8_t length, uint8_t addr)
{
    error_t err = 0;
    if (net_tx_buffer != NULL) {
        err = ERROR_NET_NOBUFS;
    } else {
        err = ERROR_OK;
    }
    // add packet regardless of error
    net_tx_buffer = {data, length};
    return err;
}

// add data to the RX buffer
error_t net_rx(uint8_t* data, uint8_t length, uint8_t addr)
{
    error_t err = 0;
    if (net_rx_buffer != NULL) {
        err = ERROR_NET_NOBUFS;
    } else {
        err = ERROR_OK;
    }
    // add packet regardless of error
    net_rx_buffer = {data, length};
    return err;
}

// do stuff with buffers, i.e. if your TX buffer has stuff, pass to DLL.
// TODO next-state logic
void net_tick(void)
{
    error_t err = 0; // error monitor

    pres_s = next_s;
    switch (pres_s) {
        case TX:
            // pass up to TRAN layer.
            err = tran_rx(
                p.tran,
                p.length-7, // size of TRAN data only
                p.src_addr // address of sender
            );
            // TODO act on returned errors
            // use if statements to determine next state
            // next_s = IDLE;
            break;
        case RX:
            // pass rx data to handler
            err = net_rx_handler(
                net_to_struct(&net_rx_buffer, sizeof(net_rx_buffer))
            );
            // TODO act on returned errors
            // use if statements to determine next state
            // next_s = IDLE;
            break;
        default:
            break;
    }
}

//---------- internal functions ----------//

error_t net_tx_handler(net_packet_t p)
{
    // TODO write this function
    err = tran_rx(
        p.tran,
        p.length-7, // size of TRAN data only
        p.src_addr // address of sender
    );
    // TODO act on returned errors
}

error_t net_rx_handler(net_packet_t p)
{
    // check checksum
    error_t err = 0;
    if ((err = valid_cksum(&p))) {
        return err;
    }

    // check address
    if (p.dest_addr != LOCAL_ADDRESS) {
        switch (p.vers) {
            case 0: // drop packet
                return ERROR_NET_DROP;
            case 1: // flooding
                // decrement hop counter, resend if hop counter ok
                if (--p.hop == 0) return ERROR_NET_DROP;
                // send packet back down to DLL to transmit.
                err = dll_tx(
                    net_to_array(*p),
                    p.length,
                    p.src_addr
                );
                // TODO act on returned errors
                return err;
            case 2: // link state routing
                // TODO Not yet implemented.
            default:
                return ERROR_OK;
        }
    }

    // packet meant for me, what do?
    switch (p.type) {
        case LSA:
            // received LSA, queue up ACK reply
            // net_tx_buffer = {
            //     net_to_array(
            //         // src->dest, dest->src (or LOCAL_ADDRESS)
            //         net_ack_packet(p.vers, p.dest_addr, p.src_addr)
            //     ),
            //     p.length
            // };

            // should I be calling this?
            err = net_tx(
                net_to_array(&p),
                p.length,
                p.src_addr
            );
            // TODO act on returned errors
            return err;
        case APP:
            // pass up to TRAN layer.
            net_tx_handler(p);
            break;
        case LSP:
            // TODO Not yet implemented.
            break;
        default:
            break;
    }

    // probably shouldn't get here
    return ERROR_OK;
}

error_t net_send_lsa(void)
{
    net_packet_t p = net_lsa_packet(VERSION, LOCAL_ADDRESS);
    net_tx(
        net_to_array(&p),
        p.length,
        LOCAL_ADDRESS
    );
}

//---------- utility functions ----------//

uint8_t *net_to_array(net_packet_t *p)
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
