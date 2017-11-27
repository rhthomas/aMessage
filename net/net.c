#include "net.c"

// add data to the TX buffer
error_t net_tx(uint8_t* data, char length)
{
    error_t err;
    if (!err = buffer_push(net_to_array(*p))) {
        return ERROR_OK;
    } else {
        return err;
    }
}

// add data to the RX buffer
error_t net_rx(uint8_t* data, char length)
{

}

// do stuff with buffers, i.e. if your TX buffer has stuff, pass to DLL.
void net_tick(void)
{
    // state machine?
    pres_s = next_s;
    switch (pres_s) {
        case IDLE:
            break;
        case TX:
            // check tx buffer
            if (buffer_size(net_tx_buffer)) {
                dll_tx(buffer_pop(net_tx_buffer));
            }
            next_s = IDLE;
            break;
        case RX:
            // check rx buffer
            if (buffer_size(net_rx_buffer)) {
                tran_rx(buffer_pop(net_rx_buffer));
            }
            next_s = IDLE;
            break;
        case OTHER:
            // other functionality
            // ...
            break;
    }
}

bytestring net_to_array(net_packet_t *p)
{

}

net_packet_t net_to_struct(uint8_t* data, uint8_t length)
{
    net_packet_t packet = {
        .ack       = (data[0] >> 7) & 0b0001,
        .type      = (data[0] >> 5) & 0b0011,
        .hop       = (data[0] >> 2) & 0b0111,
        .ver       = data[0]        & 0b0011,
        .src_addr  = data[2],
        .dest_addr = data[3],
        .len       = data[4],
        .tran      = &data[5],
        .csum      = data[data[4]-2],
    }
    return packet;
}
