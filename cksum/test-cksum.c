/**

    Testbench for checksum functions.

*/

#include "net.h"
#include "cksum.h"
#include "print.h"

int main()
{
    uint8_t inc_data[] = {
        0x1c, 0x00, 0xaa, 0xab, 0x0f, 0x00, 0x11, 0x22,
        0x33, 0x44, 0x55, 0x66, 0x77, 0x00, 0x00, 0x00,
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
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // <-- error has not effected cksum since its out of data
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    // TODO this isnt returning correctly
    net_packet_t p = net_to_struct(inc_data, sizeof(inc_data));
    printf("size: %d\n", p.length);
    print_struct(p);

    /*--------------------------------------------------------------------------
    TEST: xor_sum

    Calculate the XOR checksum of the data in inc_data[] array.

    PASS: N - Checksum output is the wrong way around. 0x1200 rather than 0x0012.
    --------------------------------------------------------------------------*/
    p.cksum  = xor_sum(&p);
    printf("cksum: 0x%04x\n", p.cksum);
    print_struct(p);

    /*--------------------------------------------------------------------------
    TEST: valid_cksum

    Check the previously validated checksum is correct.

    PASS: N
    --------------------------------------------------------------------------*/
    error_t err = 0;
    if (!(err == valid_cksum(&p))) {
        printf("ok\n");
    } else {
        printf("error\n");
    }

    return 0;
}
