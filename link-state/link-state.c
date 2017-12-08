/*

    Link-state routing functions.

*/

#include "link-state.h"

uint32_t seq = 0;

ls_packet_t ls_new_packet(uint8_t src, ls_elem_t *list)
{
    ls_packet_t p = {
        .src = src,
        .seq = seq++,
        .age = START_AGE
    };
    for (uint8_t i=0; i<MAX_TABLE_SIZE; i++) {
        p.connected[i] = list[i];
    }
    return p;
}

ls_packet_t array_to_lsp(uint8_t *array, uint8_t length)
{
    ls_packet_t p;
    for (uint8_t i=0; i<length; i++) {
        p.elem[i] = array[i];
    }
    return p;
}

net_packet_t net_lsp_packet(uint8_t src, ls_packet_t *lsp)
{
    net_packet_t p = {
        .vers = VERSION,
        .hop  = 0b111,
        .type = LSP,
        .ack  = 0,
        .res  = 0x00,
        .src_addr = src,
        .dest_addr = 0x00, // bcast
        .length = sizeof(ls_packet_t) + 7, // 7 = net overhead
        .tran = {0}, // min 8
        .cksum = 0x0000
    };

    for (uint8_t i=0; i<sizeof(ls_packet_t); i++) {
        p.tran[i] = lsp->elem[i];
    }

    // fill TRAN field
    p.cksum = xor_sum(&p);
    return p;
}

void add_ls_table(ls_packet_t lsp)
{
    for (uint8_t i=0; i<MAX_TABLE_SIZE; i++) {
        if (!(ls_list[i].occupied)) {
            ls_list[i].occupied = 1;
            ls_list[i].lsp = lsp;
            return;
        }
    }
}

void update_ls_table(void)
{
    for (uint8_t i=0; i<MAX_NODES; i++) {
        if (ls_list[i].occupied) {
            if (--ls_list[i].lsp.age == 0) {
                // node dead, free memory
                ls_list[i].occupied = 0;
            }
        }
    }
}

void shortest_path(uint8_t src, uint8_t dest, uint8_t path[])
{

}
