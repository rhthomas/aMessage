/**

    Source file for NET layer.

*/

#include "net.h"

//---------- public methods ----------//

// add data to the TX buffer
error_t net_tx(uint8_t *data, uint8_t length, uint8_t mac)
{
    error_t err = 0;
    err = net_buffer_push(
        &net_tx_buffer,
        &net_tx_size,
        data,
        length,
        mac
    );
    return err;
}

// add data to the RX buffer
error_t net_rx(uint8_t *data, uint8_t length, uint8_t mac)
{
    error_t err = 0;
    err = net_buffer_push(
        &net_rx_buffer,
        &net_rx_size,
        data,
        length,
        mac
    );
    return err;
}

// do stuff with buffers, i.e. if your TX buffer has stuff, pass to DLL.
// TODO next-state logic
void net_tick(void)
{
    error_t err = 0; // error monitor

    // TODO modulo block
    // NOTE need global count variable from PHY
    update_node_table(&known_nodes, &num_nodes);

    pres_s = next_s;
    switch (pres_s) {
        case TX:
            if (net_tx_size > 0) {
                // TODO write this
                err = net_tx_handler(
                    // tran packet in buffer
                );
                if (err) {
                    next_s = pres_s;
                    break;
                }
            }
            next_s = RX;
            break;
        case RX:
            // only do stuff if there are things in the RX buffer
            if (net_rx_size > 0) {
                // pass rx data to handler
                bytestring_t bs = net_buffer_pop(
                    &net_rx_buffer,
                    &net_rx_size
                );
                err = net_rx_handler(
                    net_to_struct(bs.data, bs.length)
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

error_t net_tx_handler(void)
{
    error_t err = 0;
    // TODO write this function
    bytestring_t bs = net_buffer_pop(&net_tx_buffer, &net_tx_size);
    // pad TRAN data with net stuff
    net_packet_t p = {
        .vers = v,
        .hop  = 0b111,
        .type = APP,
        .ack  = 0,
        .res  = 0x00,
        .src_addr  = LOCAL_ADDRESS,
        .dest_addr = bs.mac,
        .length    = bs.length + 7,
        .tran      = bs.data,
        .cksum     = 0x0000
    };
    p.cksum = xor_sum(&p);
    // pass down to dll
    err = dll_tx(
        
    );
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
                    net_to_array((net_packet_t *)&p),
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
    int8_t index; // if defined inside switch it is out of scope?
    switch (p.type) {
        case LSA:
            // search for node index, -1 if it does not exist
            index = search_list(&known_nodes, p.src_addr, &num_nodes);
            if (index < 0) {
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
                    reply.dest_addr
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

//---------- buffers ----------//

error_t net_buffer_push(bytestring_t *buffer, uint8_t *size, uint8_t *data,
    uint8_t length, uint8_t mac)
{
    if (*size >= 0 && *size < MAX_BUFFER_SIZE) {
        buffer[*size] = (bytestring_t){data, length, mac};
        *size += 1;
        return ERROR_OK;
    }
    return ERROR_NET_NOBUFS;
}

bytestring_t net_buffer_pop(bytestring_t *buffer, uint8_t *size)
{
    // store first-out data in varable
    bytestring_t out = buffer[0];
    // move all packets nearer to exit
    for (int i=0; i<*size-1; i++) {
        buffer[i] = buffer[i + 1];
    }
    // decrement size
    *size -= 1;
    // return old data
    return out;
}
