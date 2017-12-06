/*

    Link-state routing functions.

*/

#include "link-state.h"

uint32_t seq = 0;

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
