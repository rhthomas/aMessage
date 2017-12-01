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
        net_ack_packet(1, 0xAA, 0xAB)
    );
    printf("lsa packet\n"); // PASS
    print_struct(
        net_lsa_packet(1, 0xAA)
    );
    // TODO Not yet implemented.
    // print_struct(
    //     net_lsp_packet(1, 0xAA, 0xAB, /* table */);
    // );
    printf("broadcast packet\n"); // PASS
    print_struct(
        net_bcast_packet(1, 0xAA, &tran_data, sizeof(tran_data))
    );

    // incomming data from dll.
    uint8_t inc_data[] = {
        0x1c, 0x00, 0xaa, 0xab,
        0x0f, 0x00, 0x11, 0x22,
        0x33, 0x44, 0x55, 0x66,
        0x77, 0x00, 0x12
    };

    printf("incoming array\n");
    print_array(inc_data, sizeof(inc_data));

    // test net_to_struct PASS
    printf("converted to struct\n");
    net_packet_t p = net_to_struct(inc_data, sizeof(inc_data));
    print_struct(p);

    // test net_to_array FAIL
    printf("out array\n");
    uint8_t *out_data = net_to_array(&p);
    print_array(out_data, sizeof(out_data));

    return 0;
}
