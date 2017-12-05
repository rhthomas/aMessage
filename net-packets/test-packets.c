/**

    Testbench for network layer API calls and functions.

*/

#include "net.h"
#include "print.h"

int main()
{
    // noddy TRAN data
    uint8_t tran_data[] = {
        0x11, 0x22, 0x33, 0x44,
        0x55, 0x66, 0x77, 0x88,
    };

    // test making packets
    printf("ack packet\n"); // PASS
    print_struct(
        net_ack_packet(0xAA, 0xAB)
    );
    printf("lsa packet\n"); // PASS
    print_struct(
        net_lsa_packet(0xAA)
    );
    printf("broadcast packet\n"); // PASS
    print_struct(
        net_bcast_packet(0xAA, tran_data, sizeof(tran_data))
    );
    // TODO Not yet implemented.
    // print_struct(
    //     net_lsp_packet(1, 0xAA, 0xAB, /* table */);
    // );

    return 0;
}
