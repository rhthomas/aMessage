/**

    Testbench for network layer API calls and functions.

*/

#include "net.h"
#include "print.h"

// incomming data from dll.
uint8_t inc_data[] = {
    0x1c, 0x00, 0xaa, 0xab, 0x0f,
    // TRAN data
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
    // checksum
    0x00, 0x12
};

int main()
{
    // buffer to pass around the place
    bytestring_t bs;

    printf("incoming array\n");
    print_array(inc_data, sizeof(inc_data));

    // test net_to_struct PASS
    printf("converted to struct\n");
    net_packet_t p = net_to_struct(inc_data, sizeof(inc_data));
    print_struct(p);

    // test net_to_array PASS
    printf("out array\n");
    uint8_t *out_data = net_to_array(&p);
    print_array(out_data, 128);

    // test net_buffer_push PASS
    error_t err = 0;
    printf("test buffer push\n");
    // push to the buffer a few times
    for (int i=0; i<3; i++) {
        bs.length = sizeof(inc_data);
        bs.mac = inc_data[3];
        for (int j=0; j<121; j++) {
            bs.data[j] = inc_data[j] + i;
        }
        err = net_buffer_push(&net_tx_buffer, bs);
        if (!err) {
            printf("ok: %d\n", net_buffer_size(&net_tx_buffer));
            print_array(net_tx_buffer.buffer[i].data, net_tx_buffer.buffer[i].length);
        } else {
            printf("error: %d\n", err);
        }
    }
    printf("\n");

    // test net_buffer_peak
    printf("test buffer peak\n");
    // bytestring_t bs;
    for (int i=0; i<3; i++) {
        err = net_buffer_peak(&net_tx_buffer, &bs);
        if (!err) {
            printf("ok: %d\n", net_buffer_size(&net_tx_buffer));
            print_array(bs.data, bs.length);
        } else {
            printf("error: %d\n", err);
        }
    }
    printf("\n");

    // test net_buffer_pop PASS
    printf("test buffer pop\n");
    // bytestring_t bs;
    for (int i=0; i<8; i++) {
        err = net_buffer_pop(&net_tx_buffer, &bs);
        if (!err) {
            printf("ok: %d\n", net_buffer_size(&net_tx_buffer));
            print_array(bs.data, bs.length);
        } else {
            printf("error: %d\n", err);
        }
    }
    printf("\n");

    return 0;
}
