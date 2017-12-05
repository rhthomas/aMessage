/**

    Testbench for network layer API calls and functions.

*/

#include "net.h"
#include "print.h"

int main()
{
    // incomming data from dll.
    uint8_t inc_data[] = {
        0x1c, 0x00, 0xaa, 0xab, 0x0f,

        0x00, 0x11, 0x22,
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
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

        0x00, 0x12
    };

    printf("incoming array\n");
    print_array(inc_data, sizeof(inc_data));

    // test net_to_struct FAIL - random data after real tran stuff
    printf("converted to struct\n");
    net_packet_t p = net_to_struct(inc_data, sizeof(inc_data));
    print_struct(p);

    // test net_to_array FAIL - random data after first byte
    printf("out array\n");
    uint8_t *out_data = net_to_array(&p);
    print_array(out_data, sizeof(net_packet_t));

    // test net_buffer_push PASS
    error_t err = 0;
    printf("test buffer push\n");
    for (int i=0; i<3; i++) {
        err = net_buffer_push(
            &net_tx_buffer,
            &net_tx_size,
            &inc_data,
            sizeof(inc_data),
            inc_data[3]
        );
        if (!err) {
            printf("ok\n");
        } else {
            printf("error: %d\n", err);
        }
    }
    printf("\n");

    // test net_buffer_pop FAIL
    printf("test buffer pop\n");
    bytestring_t bs = net_buffer_pop(&net_tx_buffer, &net_tx_size);
    print_array(bs.data, bs.length);

    return 0;
}
