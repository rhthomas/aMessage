#include "net-packets.h"

net_packet_t net_ack_packet(uint8_t v, uint8_t src, uint8_t dest)
{
    net_packet_t p = {
        .vers = v,
        .hop  = 0b111,
        .type = LSA, // replying to LSA from new node
        .ack  = 0b1,
        .res  = 0x00,
        .src_addr = src,
        .dest_addr = dest,
        .length = 0x00,
        .tran = 0x00, // min 8?
        .cksum = 0x0000
    };
    p.cksum = xor_sum(&p);
    p.length = 8; // ?
    return p;
}

net_packet_t net_lsa_packet(uint8_t v, uint8_t src)
{
    net_packet_t p = {
        .vers = v,
        .hop  = 0b111,
        .type = LSA,
        .ack  = 0,
        .res  = 0x00,
        .src_addr = src,
        .dest_addr = 0x00, // bcast
        .length = 0x00,
        .tran = 0x00, // min 8? I dont want this field.
        .cksum = 0x0000
    };
    p.cksum = xor_sum(&p);
    p.length = 15; // ?
    return p;
}

// net_packet_t net_lsp_packet(uint8_t v, uint8_t src, uint8_t dest,
//     ls_table_t *table)
// {
//
// }

net_packet_t net_bcast_packet(uint8_t v, uint8_t src, uint8_t *data,
    uint8_t data_len)
{
    net_packet_t p = {
        .vers = v,
        .hop  = 0b111,
        .type = APP,
        .ack  = 0,
        .res  = 0x00,
        .src_addr = src,
        .dest_addr = 0x00, // bcast address
        .length = 0x00,
        .tran = data, // ?
        .cksum = 0x0000
    };
    p.cksum = xor_sum(&p);
    p.length = data_len + 7;
    return p;
}
