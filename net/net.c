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

    // TODO modulo block
    update_node_table(&knwon_nodes, &num_nodes);

    pres_s = next_s;
    switch (pres_s) {
        case TX:
            if (net_tx_buffer) {
                // TODO handle TX packets
                if (err) {
                    next_s = pres_s;
                    break;
                }
            }
            next_s = RX;
            break;
        case RX:
            // only do stuff if there are things in the RX buffer
            if (net_rx_buffer) {
                // pass rx data to handler
                err = net_rx_handler(
                    net_to_struct(&net_rx_buffer, sizeof(net_rx_buffer))
                );
                // if error, try this again on next tick?
                if (err) {
                    next_s = pres_s;
                    break;
                }
            }
            next_s = TX;
            break;
        default:
            break;
    }
}

//---------- internal functions ----------//

error_t net_tx_handler(net_packet_t p)
{
    // TODO write this function
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
            // search for node index, -1 if it does not exist
            int8_t index = search_list(&known_nodes, p.src_addr, &num_nodes);
            if (index == -1) {
                // add to list of known nodes
                new_node(
                    &known_nodes,
                    (node_t){p.src_addr, START_AGE},
                    &num_nodes
                );
            } else {
                // node in list, update age
                known_nodes[index].age = START_AGE;
            }
            // reply if original message was NOT already an ACK
            if (p.ack == 0) {
                // received LSA, queue up ACK reply
                net_packet_t reply = net_ack_packet(
                    p.vers,
                    // reverse addresses
                    p.dest_addr,
                    p.src_addr // LOCAL_ADDRESS
                );
                // add to tx buffer
                err = dll_tx(
                    net_to_array(&reply),
                    reply.length,
                    reply.dest_addr // ?
                );
                return err;
            }
            // if it was already an ACK, just add node and return
            return ERROR_OK;
        case APP:
            // pass up to TRAN layer.
            err = tran_rx(
                p.tran,
                p.length - 7, // 7 bytes of NET stuff
                p.src_addr
            );
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
    dll_tx(
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
