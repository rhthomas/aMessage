/**

    Testbench for network layer API calls and functions.

*/

#include "net.h"
#include "print.h"

int main()
{
    // example TRAN data
    uint8_t tran_data[121] = {
        // control[2], src[1], dest[1], length[1]
        0x11, 0x11, 0x22, 0x33, 0x08,
        // app[114] only one byte used
        0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,
        // checksum[2]
        0x00, 0x5d
    };

    /*--------------------------------------------------------------------------
    TEST: net_ack_packet

    Expected control byte of 0x9c = 0b10011100.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("ack packet\n");
    print_struct(
        net_ack_packet(0xAA, 0xAB)
    );

    /*--------------------------------------------------------------------------
    TEST: net_lsa_packet

    Expected control byte of 0x1c = 0b00011100.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("lsa packet\n");
    print_struct(
        net_lsa_packet(0xAA)
    );

    /*--------------------------------------------------------------------------
    TEST: net_bcast_packet

    Expected control byte of 0x3c = 0b00111100.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("broadcast packet\n");
    print_struct(
        net_bcast_packet(0xAA, tran_data, tran_data[4])
    );

    /*--------------------------------------------------------------------------
    TEST: net_data_packet

    Expected control byte of 0x3c = 0b00111100.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("tran data packet\n");
    print_struct(
        net_data_packet(
            0xAA,
            tran_data[3],
            tran_data,
            tran_data[4]
        )
    );

    return 0;
}
