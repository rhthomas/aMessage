/**

    Testbench for network layer API calls and functions.

*/

#include "net.h"
#include "print.h"

// incomming data from dll.
uint8_t inc_data[] = {
    // control[2], src[1], dest[1], length[1]
    0x1c, 0x00, 0xaa, 0xab, 0x0f,
    // TRAN data[121]
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
    // checksum[2]
    0x00, 0x12
};

int main()
{
    // buffer to pass around the place
    bytestring_t bs;

    printf("incoming array\n");
    print_array(inc_data, sizeof(inc_data));

    /*--------------------------------------------------------------------------
    TEST: net_to_struct

    Convert the inc_data[] array to a structure and print the contents.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("converted to struct\n");
    net_packet_t p = net_to_struct(inc_data, sizeof(inc_data));
    print_struct(p);

    /*--------------------------------------------------------------------------
    TEST: net_to_array

    Take the preivously converted structure and turn it back into an array,
    again printing the contents.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("out array\n");
    uint8_t *out_data = net_to_array(&p);
    print_array(out_data, 128);

    /*--------------------------------------------------------------------------
    TEST: net_buffer_push

    Attempt to push 3 elements to the net_tx_buffer which is two elements wide.
    Two should succeed with the buffer contents printed, one should fail with
    error = 3, ERROR_NET_NOBUFS.

    PASS: Y
    --------------------------------------------------------------------------*/
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

    /*--------------------------------------------------------------------------
    TEST: net_buffer_peak

    Attempt to peak at the net_tx_buffer 3 times. Should return the same data
    every time since peak shows the oldest buffer data (FIFO). Would only return
    an error if the buffer was empty. This condition is not tested here.

    PASS: Y
    --------------------------------------------------------------------------*/
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

    /*--------------------------------------------------------------------------
    TEST: net_buffer_pop

    Try to pop data from the buffer 8 times. This should succeed for 2 attempts
    since there are two elements in the buffer, and fail for the other 6 returning
    ERROR_NET_NOBUFS.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("test buffer pop\n");
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
