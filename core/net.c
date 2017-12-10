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
    //print_array(bs.data, bs.length);
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
    //print_array(bs.data, bs.length);
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
    error = net_tx_handler();
/*    if(error != ERROR_OK){
//        put_str(" [NET] TX Not OK\r\n");
        put_ch('T');
    }
    else{
//        put_str(" [NET] TX OK :) \r\n");
        put_ch('t');
    }*/
    error = net_rx_handler();
/*    if(error != ERROR_OK){
//        put_str(" [NET] RX Not OK\r\n");
        put_ch('R');
    }
    else{
//        put_str(" [NET] RX OK :) \r\n");
        put_ch('r');

    }*/
}

//---------- internal functions ----------//

error_t net_tx_handler(void)
{
    error_t err = 0;

    // pop data from tx handler
    if ((err = net_buffer_peek(&net_tx_buffer, &pass))) {
        return err;
    }
    //print_array(pass.data, pass.length);

    // TODO search node tables or ls_list
    // TODO routing dll_tx(data, len, first node in list)

    // check node exists. if not, return
    if (VERSION > 0) {
        if (pass.mac != 0x00) {
            if (search_list(pass.mac) == -1) {
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
    //print_struct(p);

//    put_str(" [NET] Tx Checksum: ");
//    put_hex(((uint8_t*)&p.cksum)[1]);
//    put_str("\r\n");

    // pass down to dll, return error
    if ((err = dll_tx(net_to_array(&p, p.length), p.length, p.dest_addr))) {
        // there was an error
        put_str(" [NET] dll_tx error\r\n");
        return err;
    }

    // everything is fine, pop and drop the packet
    net_buffer_pop(&net_tx_buffer, &pass);
    //put_str(" [NET] dll_tx no errors\r\n");
    return ERROR_OK;
}

error_t net_rx_handler(void)
{
    error_t err = 0;

    // pop data out of rx buffer
    if ((err = net_buffer_peek(&net_rx_buffer, &pass))) {
        return err;
    }
    //print_array(pass.data, pass.length);

    // convert to packet
    net_packet_t p = net_to_struct(pass.data, pass.length);
    //print_struct(p);

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
            put_str(" [NET] flooding packet\r\n");
            // decrement hop counter, resend if hop counter ok
            if (--p.hop == 0) {
                net_buffer_pop(&net_rx_buffer, &pass);
                put_str(" [NET] hop==0, packet dropped\r\n");
                return ERROR_NET_DROP;
            }
            // send packet back down to DLL to transmit.
            err = dll_tx(
                net_to_array(&p, p.length),
                p.length,
                p.dest_addr
            );
/*            if (!err) {
                net_buffer_pop(&net_rx_buffer, &pass);
            }*/
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
            // pass up to TRAN layer.
            err = tran_rx(
                p.tran,
                p.length - 7, // 7 bytes of NET stuff
                p.src_addr
            );
            //if (!err) {
                net_buffer_pop(&net_rx_buffer, &pass);
                put_str(" [NET] passed up to TRAN\r\n");
            //}
            return err;
        case LSP:
            // add packet to table
            add_ls_table(array_to_lsp(p.tran, 21));
            //print_struct(net_lsp_packet(dll_address, &ls_list[0].lsp));
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
/*        if (i == p->length-3) {
            // skip to end of array
            i = 125;
            // 2 bytes left after i++ for cksum
        }*/
    }
    net_data[length-2] = p->elem[128-1];
    net_data[length-1] = p->elem[128-2];
    return net_data;
}

net_packet_t net_to_struct(uint8_t *data, uint8_t length)
{
    //#define METHOD
    #ifdef METHOD
    net_packet_t packet;
    for (int i=0; i<length; i++) {
        packet.elem[i] = data[i];
        // skip any unused TRAN data
        if (i == data[4]-3) {
            // jump to cksum field
            i = length-3; //125;
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
        .cksum = data[length-1] | (data[length-2]<<8)
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
    #ifdef DEBUG
    printf("[NET]\tbuffer is full\r\n");
    #endif
    put_str(" [NET] buffer is full\r\n");
    return ERROR_NET_NOBUFS;
}

error_t net_buffer_pop(net_buffer_t *buf, bytestring_t *out_bs)
{
    // return if the buffer is empty
    if (buf->head == buf->tail) {
        #ifdef DEBUG
        printf("[NET]\t buffer has nothing to pop\r\n");
        #endif
        //put_str(" [NET] buffer has nothing to pop\r\n");
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
        #ifdef DEBUG
        printf("[NET]\tbuffer has nothing to peek\r\n");
        #endif
        //put_str(" [NET] buffer has nothing to peek\r\n");
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

// error_t dll_tx(uint8_t *data, uint8_t length, uint8_t dest)
// {
//     return ERROR_OK;
// }

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
