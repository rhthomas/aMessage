/**

    Testbench for network layer API calls and functions.

*/

#include "net.h"
#include "print.h"

int main()
{
    // incomming data from dll.
    uint8_t inc_data[] = {
        0x1c, 0x00, 0xaa, 0xab,
        0x0f, 0x00, 0x11, 0x22,
        0x33, 0x44, 0x55, 0x66,
        0x77, 0x00, 0x12
    };

    printf("test packet\n");
    print_array(inc_data, sizeof(inc_data));

    // test net_to_struct PASS
    net_packet_t p = net_to_struct(inc_data, sizeof(inc_data));
    print_struct(&p);

    // test net_to_array FAIL
    uint8_t *out_data = net_to_array(&p);
    printf("out array\n");
    print_array(out_data, sizeof(out_data));

    return 0;
}
