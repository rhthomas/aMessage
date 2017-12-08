/**

    Testbench for network layer API calls and functions.

*/

#include "net.h"
#include "print.h"

// error handler
error_t err = 0;

// NET packet coming from DLL. RX
uint8_t net_data[128] = {
    // control[2], src[1], dest[1], length[1]
    0x1c, 0x00, 0xab, 0xaa, 0x0f,
    // TRAN data[121]
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
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
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00,
    // checksum[2]
    0x00, 0x12
};

// TRAN packet coming from TRAN. TX
uint8_t tran_data[121] = {
    // control[2], src[1], dest[1], length[1]
    0x11, 0x11, 0x22, 0x33, 0x08,
    // app[114]
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

// buffer to pass around the place
bytestring_t bs;

int main()
{
    printf("initial array\n");
    print_array(net_data, sizeof(net_data));

    /*--------------------------------------------------------------------------
    TEST: net_to_struct

    Convert the inc_data[] array to a structure and print the contents.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("net_to_struct\n");
    net_packet_t p = net_to_struct(net_data, sizeof(net_data));
    print_struct(p);

    /*--------------------------------------------------------------------------
    TEST: net_to_array

    Take the preivously converted structure and turn it back into an array,
    again printing the contents.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("net_to_array\n");
    uint8_t *out_data = net_to_array(&p);
    print_array(out_data, 128);

    /*--------------------------------------------------------------------------
    TEST: net_buffer_push

    Attempt to push 3 elements to the net_tx_buffer which is two elements wide.
    Two should succeed with the buffer contents printed, one should fail with
    error = 3, ERROR_NET_NOBUFS.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("net_buffer_push\n");
    // push to the buffer a few times
    for (int i=0; i<3; i++) {
        bs.length = sizeof(tran_data);
        bs.mac = tran_data[3];
        for (int j=0; j<128; j++) {
            bs.data[j] = tran_data[j] + i;
        }
        err = net_buffer_push(&net_tx_buffer, bs);
        if (!err) {
            printf("[ push ] %d item(s) in buffer\n", net_buffer_size(&net_tx_buffer));
            print_array(net_tx_buffer.buffer[i].data, net_tx_buffer.buffer[i].length);
        } else {
            printf("[ push ] error %d\n", err);
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
    printf("net_buffer_peak\n");
    // bytestring_t bs;
    for (int i=0; i<3; i++) {
        err = net_buffer_peak(&net_tx_buffer, &bs);
        if (!err) {
            printf("[ peak ] %d item(s) in buffer\n", net_buffer_size(&net_tx_buffer));
            print_array(bs.data, bs.length);
        } else {
            printf("[ peak ] error %d\n", err);
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
    printf("net_buffer_pop\n");
    for (int i=0; i<8; i++) {
        // err = net_buffer_pop(&net_tx_buffer, &bs);
        if (!(err = net_buffer_pop(&net_tx_buffer, &bs))) {
            printf("[ pop  ] %d item(s) in buffer\n", net_buffer_size(&net_tx_buffer));
            print_array(bs.data, bs.length);
        } else {
            printf("[ pop  ] error %d\n", err);
        }
    }
    printf("\n");

    /*--------------------------------------------------------------------------
    TEST: net_tx

    Tests API for other layers. This function is called by the TRAN layer and it
    passes down the outgoing TRAN packet to add to the net_tx_buffer.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("net_tx\n");
    // push data to tx buffer
    err = net_tx(tran_data, tran_data[4], 0xAB);
    if (!err) {
        printf("[ TX  ] net_tx no errors\n");
    } else {
        printf("[ TX  ] net_tx error %d\n", err);
    }
    printf("\n");

    /*--------------------------------------------------------------------------
    TEST: net_tx_handler

    Add data to the tx buffer, then call the tx handler with debugging print
    functions.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("net_tx_handler\n");
    err = net_tx_handler();
    if (!err) {
        printf("[ TX  ] net_tx_handler no errors\n");
    } else {
        printf("[ TX  ] net_tx_handler error %d\n", err);
    }
    printf("\n");

    /*--------------------------------------------------------------------------
    TEST: net_rx

    Tests API for other layers. This function is called by the DLL layer and it
    passes up the incoming NET packet to add to the net_rx_buffer.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("net_rx\n");
    // push data to rx buffer
    err = net_rx(net_data, sizeof(net_data), 0xAB);
    if (!err) {
        printf("[ RX  ] net_rx no errors\n");
    } else {
        printf("[ RX  ] net_rx error %d\n", err);
    }
    printf("\n");

    /*--------------------------------------------------------------------------
    TEST: net_rx_handler

    Add data to the rx buffer, then call the tx handler with debugging print
    functions.

    PASS: Y
    --------------------------------------------------------------------------*/
    printf("net_rx_handler\n");
    err = net_rx_handler();
    if (!err) {
        printf("[ RX  ] net_rx_handler no errors\n");
    } else {
        printf("[ RX  ] net_rx_handler error %d\n", err);
    }
    printf("\n");

    /*--------------------------------------------------------------------------
    TEST: link state stuff



    PASS: Y
    --------------------------------------------------------------------------*/
    uint8_t lsp_data[128] = {
        0x5e, 0x00, 0xaa, 0x00, 0x1c,

        0xaa, 0x00, 0x00,
        0x00, 0x00, 0x88, 0x13, 0xab, 0x01, 0xac, 0x01,
        0xba, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,

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
        0x00, 0x00, 0x00, 0x00,
        // checksum[2]
        0x00, 0x66
    };
    printf("link state stuff\n");
    print_array(lsp_data, 128);

    while (net_buffer_size(&net_rx_buffer)) {
        net_buffer_pop(&net_rx_buffer, &bs);
    }

    // push data to tx buffer
    err = net_rx(lsp_data, sizeof(lsp_data), lsp_data[2]);
    if (!err) {
        printf("[ RX  ] net_rx no errors\n");
    } else {
        printf("[ RX  ] net_rx error %d\n", err);
    }
    printf("\n");

    err = net_rx_handler();
    if (!err) {
        printf("[ RX  ] net_rx_handler no errors\n");
    } else {
        printf("[ RX  ] net_rx_handler error %d\n", err);
    }
    printf("\n");

    return 0;
}
