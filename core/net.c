/**

    Source file for NET layer.

*/

#include "net.h"
#include "uart.h"
//#include "print.h"
#include "dll.h"

//---------- global variables ----------//

bytestring_t pass; ///< Global bytestring for passing around.
uint8_t net_data[128]; ///< Global array for passing around.
uint16_t timer_count; ///< Tick counter from timer every 10ms.
extern uint8_t dll_address;

//---------- public methods ----------//

error_t net_tx(uint8_t *data, uint8_t length, uint8_t mac)
{
    // fill new bytestring
    bytestring_t bs;
    bs.length = length;
    bs.mac = mac;
    for (int i=0; i<121; i++) {
        bs.data[i] = data[i];
    }
    // push to tx buffer
    return net_buffer_push(&net_tx_buffer, bs);
}

error_t net_rx(uint8_t *data, uint8_t length, uint8_t mac)
{
    // fill new bytestring
    bytestring_t bs;
    bs.length = length;
    bs.mac = mac;
    for (int i=0; i<128; i++) {
        bs.data[i] = data[i];
    }
    // push to rx buffer
    return net_buffer_push(&net_rx_buffer, bs);
}

void net_tick(void)
{
    if (!(timer_count % 3000)) {
        // timer ticks every 10ms, send LSA every 3s
        put_str("\x1b[1;36m [NET] LSA Sent!\r\n\x1b[0m");
        net_send_lsa();
    }

    // update tables
    update_node_table();
    if (VERSION == 2) update_ls_table();

    error_t error;

    // handlers
    net_tx_handler();
    net_rx_handler();
}

//---------- internal functions ----------//

error_t net_tx_handler(void)
{
    error_t err = 0;

    // pop data from tx handler
    if ((err = net_buffer_peek(&net_tx_buffer, &pass))) {
        return err;
    }

    // check node exists. if not, return
    if (VERSION > 0) {
        if (pass.mac != 0x00) {
            if (search_list(pass.mac) == -1) {
                net_buffer_pop(&net_tx_buffer, &pass);
                return ERROR_NODE_UNKNOWN;
            }
        }
    }

    // pad TRAN data with net stuff
    net_packet_t p = net_data_packet(
        dll_address,
        pass.mac,
        pass.data,
        pass.length
    );

    // pass down to dll, return error
    if ((err = dll_tx(net_to_array(&p, p.length), p.length, p.dest_addr))) {
        // there was an error
        put_str(" [NET] dll_tx error\r\n");
        return err;
    }

    // everything is fine, pop and drop the packet
    net_buffer_pop(&net_tx_buffer, &pass);
    return ERROR_OK;
}

error_t net_rx_handler(void)
{
    error_t err = 0;

    // pop data out of rx buffer
    if ((err = net_buffer_peek(&net_rx_buffer, &pass))) {
        return err;
    }

    // convert to packet
    net_packet_t p = net_to_struct(pass.data, pass.length);

    // check checksum
    if ((err = valid_cksum(&p))) {
        net_buffer_pop(&net_rx_buffer, &pass);
        put_str(" [NET] Bad Checksum!\r\n");
        return err;
    }

    // check address isn't local or bcast
    if ((p.dest_addr != dll_address) && (p.dest_addr != 0)) {
        put_str(" [NET] not for me or bcast\r\n");
        if (p.vers == 0) {
            put_str(" [NET] dropping packet\r\n");
            net_buffer_pop(&net_rx_buffer, &pass);
            return ERROR_NET_DROP;
        } else {
            // decrement hop counter, resend if hop counter ok
            if (--p.hop == 0) {
                net_buffer_pop(&net_rx_buffer, &pass);
                put_str(" [NET] hop==0, packet dropped\r\n");
                return ERROR_NET_DROP;
            }
            put_str(" [NET] flooding packet\r\n");
            // send packet back down to DLL to transmit.
            p.cksum = xor_sum(&p);
            err = dll_tx(
                net_to_array(&p, p.length),
                p.length,
                p.dest_addr
            );
            net_buffer_pop(&net_rx_buffer, &pass);
            return err;
        }
    }

    put_str(" [NET] packet for me\r\n");
    // packet meant for me, what do?
    int8_t index; // if defined inside switch it is out of scope?
    switch (p.type) {
        case LSA:
            // search for node index, -1 if it does not exist
            index = search_list(p.src_addr);
            if (index < 0) {
                // add to list of known nodes
                //put_str(" [NET] Adding new node.\r\n");
                new_node((node_t){p.src_addr, START_AGE});
            } else {
                // node in list, update age
                //put_str(" [NET] Updating node age.\r\n");
                known_nodes[index].node.age = START_AGE;
            }
            // reply if original message was NOT already an ACK
            if (p.ack == 0) {
                // received LSA, queue up ACK reply
                net_packet_t reply = net_ack_packet(
                    // reverse addresses
                    dll_address,
                    p.src_addr
                );
                put_str(" [NET] send ACK down to DLL\r\n");
                // add to tx buffer
                err = dll_tx(
                    net_to_array(&reply, reply.length),
                    reply.length,
                    reply.dest_addr
                );
                // data not needed, pop before return
                net_buffer_pop(&net_rx_buffer, &pass);
                return err;
            }
            // if it was already an ACK, just add node and return
            // data not needed, pop before return
            net_buffer_pop(&net_rx_buffer, &pass);
            return ERROR_OK;
        case APP:
            put_str(" [NET] passed up to TRAN\r\n");
            // pass up to TRAN layer.
            err = tran_rx(
                p.tran,
                p.length - 7, // 7 bytes of NET stuff
                p.src_addr
            );
            net_buffer_pop(&net_rx_buffer, &pass);
            return err;
        case LSP:
            // add packet to table
            add_ls_table(array_to_lsp(p.tran, 21));
            put_str(" [NET] added LSP to table\r\n");
            net_buffer_pop(&net_rx_buffer, &pass);
            break;
        default:
            break;
    }

    // probably shouldn't get here
    net_buffer_pop(&net_rx_buffer, &pass);
    return ERROR_OK;
}

error_t net_send_lsa(void)
{
    net_packet_t p = net_lsa_packet(dll_address);
    return dll_tx(
        net_to_array(&p, p.length),
        p.length,
        p.dest_addr // bcast
    );
}

//---------- utility functions ----------//

uint8_t *net_to_array(net_packet_t *p, uint8_t length)
{
    // uint8_t data[128]; // <-- needs to be global
    for (uint8_t i=0; i<length; i++) {
        net_data[i] = p->elem[i];
    }
    net_data[length-2] = p->elem[128-1];
    net_data[length-1] = p->elem[128-2];
    return net_data;
}

net_packet_t net_to_struct(uint8_t *data, uint8_t length)
{
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
        .cksum = data[length-1] | (data[length-2]<<8)
    };
    // fill tran field
    // p.length is length of whole packet with 7 bytes for NET fields.
    for (int i=0; i<packet.length-7; i++) {
        packet.tran[i] = data[5+i];
    }
    return packet;
}

//---------- buffers ----------//

error_t net_buffer_push(net_buffer_t *buf, bytestring_t bs)
{
    uint8_t next = buf->head + 1;
    if (next >= MAX_BUFFER_SIZE) {
        next = 0;
    }
    // check buffer is not full
    if (next != buf->tail) {
        // copy bs to buffer location
        for (uint8_t i=0; i<128; i++) {
            buf->buffer[buf->head].data[i] = bs.data[i];
        }
        buf->buffer[buf->head].length = bs.length;
        buf->buffer[buf->head].mac = bs.mac;
        // increment
        buf->head = next;
        return ERROR_OK;
    }
    put_str(" [NET] buffer is full\r\n");
    return ERROR_NET_NOBUFS;
}

error_t net_buffer_pop(net_buffer_t *buf, bytestring_t *out_bs)
{
    // return if the buffer is empty
    if (buf->head == buf->tail) {
        return ERROR_NET_NOBUFS;
    }
    // buffer not empty, send data to out_bs
    uint8_t next = buf->tail + 1;
    if (next >= MAX_BUFFER_SIZE) {
        next = 0;
    }
    *out_bs = buf->buffer[buf->tail];
    buf->tail = next;
    return ERROR_OK;
}

error_t net_buffer_peek(net_buffer_t *buf, bytestring_t *out_bs)
{
    // return if the buffer is empty
    if (buf->head == buf->tail) {
        return ERROR_NET_NOBUFS;
    }
    // buffer not empty, send data to out_bs
    *out_bs = buf->buffer[buf->tail];
    return ERROR_OK;
}

uint8_t net_buffer_size(net_buffer_t *buf)
{
    return (buf->head - buf->tail);
}

//---------- temp functions ----------//

error_t tran_rx(uint8_t *data, uint8_t length, uint8_t src)
{
    uint8_t idx;
    put_str(" [TRN] From: ");
    put_hex(src);
    put_str(" got data: ");
    for(idx = 0; idx < length; idx++){
        put_ch(data[idx]);
    }
    put_str("\r\n");
    return ERROR_OK;
}
