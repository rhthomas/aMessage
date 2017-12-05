/**

    Source file for NET layer.

*/

#include "net.h"

//---------- global variables ----------//

uint8_t net_tx_size = 0;
uint8_t net_rx_size = 0;

//---------- public methods ----------//

// add data to the TX buffer
error_t net_tx(uint8_t *data, uint8_t length, uint8_t mac)
{
    error_t err = 0;
    err = net_buffer_push(
        net_tx_buffer,
        &net_tx_size,
        data,
        length,
        mac
    );
    return err;
    // if (net_tx_size == 0) {
    //     net_tx_buffer = (bytestring_t){{*data}, length, mac};
    //     net_tx_size++;
    //     return ERROR_OK;
    // }
    // return ERROR_NET_NOBUFS;
}

// add data to the RX buffer
error_t net_rx(uint8_t *data, uint8_t length, uint8_t mac)
{
    error_t err = 0;
    err = net_buffer_push(
        net_rx_buffer,
        &net_tx_size,
        data,
        length,
        mac
    );
    return err;
    // if (net_rx_size == 0) {
    //     net_rx_buffer = (bytestring_t){{*data}, length, mac};
    //     net_rx_size++;
    //     return ERROR_OK;
    // }
    // return ERROR_NET_NOBUFS;
}

// do stuff with buffers, i.e. if your TX buffer has stuff, pass to DLL.
void net_tick(void)
{
    error_t err = 0; // error monitor

    // TODO do something with this error?
    err = net_send_lsa(); // do this every 3s or something
    update_node_table();

    pres_s = next_s;
    switch (pres_s) {
        case TX:
            err = net_tx_handler();
            if (err) {
                next_s = pres_s;
                break;
            }
            next_s = RX;
            break;
        case RX:
            err = net_rx_handler();
            if (err) {
                next_s = pres_s;
                break;
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
    // return if the buffer is empty
    if (net_tx_size == 0) {
        return ERROR_OK;
    }

    // TODO routing in here when LSA is working
    bytestring_t bs = net_buffer_pop(net_tx_buffer, &net_tx_size);

    // pad TRAN data with net stuff
    net_packet_t p = {
        .vers = VERSION,
        .hop  = 0b111,
        .type = APP,
        .ack  = 0,
        .res  = 0x00,
        .src_addr  = LOCAL_ADDRESS,
        .dest_addr = bs.mac,
        .length    = bs.length + 7,
        .tran      = {*bs.data},
        .cksum     = 0x0000
    };
    p.cksum = xor_sum(&p);

    // pass down to dll, return error
    return dll_tx(
        net_to_array(&p),
        p.length,
        p.dest_addr
    );
}

error_t net_rx_handler(void)
{
    // return if the buffer is empty
    if (net_rx_size == 0) {
        return ERROR_OK;
    }

    // pop data out of rx buffer
    bytestring_t bs = net_buffer_pop(
        net_rx_buffer,
        &net_rx_size
    );
    // bytestring_t bs = net_rx_buffer;
    // net_rx_size--;

    net_packet_t p = net_to_struct(bs.data, bs.length);

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
                // add to table and continue flooding
            default:
                return ERROR_OK;
        }
    }

    // packet meant for me, what do?
    int8_t index; // if defined inside switch it is out of scope?
    switch (p.type) {
        case LSA:
            // search for node index, -1 if it does not exist
            index = search_list(p.src_addr);
            if (index < 0) {
                // add to list of known nodes
                new_node((node_t){p.src_addr, START_AGE});
            } else {
                // node in list, update age
                known_nodes[index].node.age = START_AGE;
            }
            // reply if original message was NOT already an ACK
            if (p.ack == 0) {
                // received LSA, queue up ACK reply
                net_packet_t reply = net_ack_packet(
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
            return err;
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
    net_packet_t p = net_lsa_packet(LOCAL_ADDRESS);
    return dll_tx(
        net_to_array(&p),
        p.length,
        LOCAL_ADDRESS
    );
}

//---------- utility functions ----------//

uint8_t data[128];
uint8_t *net_to_array(net_packet_t *p)
{
    // uint8_t data[128]; // <-- needs to be global
    for (uint8_t i=0; i<128; i++) {
        data[i] = p->elem[i];
        if (i == p->length-2) {
            // skip to end of array
            i = 125;
            // 2 bytes left after i++ for cksum
        }
    }
    return data;
}

net_packet_t net_to_struct(uint8_t *data, uint8_t length)
{
    // #define METHOD
    #ifdef METHOD
    net_packet_t packet;
    for (int i=0; i<length; i++) {
        packet.elem[i] = data[i];
        // skip any unused TRAN data
        if (i == data[4]-2) {
            // jump to cksum field
            i = 125;
        }
    }
    #else
    net_packet_t packet = {
        .vers = (data[0] & 0b00000011),
        .hop  = (data[0] & 0b00011100) >> 2,
        .type = (data[0] & 0b01100000) >> 5,
        .ack  = (data[0] & 0b10000000) >> 7,
        .res  = data[1],
        .src_addr = data[2],
        .dest_addr = data[3],
        .length = data[4],
        .tran = {0}, // init
        // .cksum = data[127] << 8 // backwards?
        .cksum = *(uint16_t *)&data[126]
    };
    // fill tran field
    // p.length is length of whole packet with 7 bytes for NET fields.
    for (int i=0; i<packet.length-7; i++) {
        packet.tran[i] = data[5+i];
    }
    #endif // METHOD
    return packet;
}

//---------- buffers ----------//

error_t net_buffer_push(bytestring_t *buffer, uint8_t *size, uint8_t *data,
    uint8_t length, uint8_t mac)
{
    if (*size >= 0 && *size < MAX_BUFFER_SIZE) {
        for (int i=0; i<121; i++) {
            buffer[*size].data[i] = data[i];
        }
        buffer[*size].length = length;
        buffer[*size].mac = mac;
        *size += 1;
        return ERROR_OK;
    }
    return ERROR_NET_NOBUFS;
}

// do not call if *size==0
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

//---------- temp functions ----------//

error_t dll_tx(uint8_t *data, uint8_t length, uint8_t dest)
{
    return ERROR_OK;
}

error_t tran_rx(uint8_t *data, uint8_t length, uint8_t src)
{
    return ERROR_OK;
}
