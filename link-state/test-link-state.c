/**

    Testbench for network layer API calls and functions.

*/

#include "link-state.h"
#include "print.h"

uint8_t test_lsp_array[] = {
    0xaa, 0x02, 0x00, 0x00, 0x00, 0xff, 0xff, 0xab,
    0x01, 0xac, 0x01, 0xba, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00
};

int main()
{
    ls_elem_t conn[MAX_TABLE_SIZE] = {
        {0xAB, 1},
        {0xAC, 1},
        {0xBA, 2}
    };

    for (int i=0; i<3; i++) {
        ls_packet_t p = ls_new_packet(0xAA, conn);
        add_ls_table(p); // adds to ls_list
        print_struct(net_lsp_packet(0xAA, &ls_list[i].lsp));
    }

    ls_packet_t q = array_to_lsp(test_lsp_array, sizeof(test_lsp_array));
    printf("src : %X\nseq : %d\nage : %X\n",
        q.src, q.seq, q.age
    );
    for (uint8_t i=0; i<MAX_TABLE_SIZE; i++) {
        printf("{%X, %d}", q.connected[i].addr, q.connected[i].hops);
    }
    printf("\n");

    return 0;
}
