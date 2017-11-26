#include "net.c"

// create NET packet and send to DLL, called by NET?
error_t net_tx(char* data, char length)
{

}

// get NET packet from DLL, called by DLL
error_t net_rx(char* data, char length)
{
    // push data to net_rx_buffer
}

// do stuff with buffers, i.e. if your TX buffer has stuff, pass to DLL.
void net_tick(void)
{
    // check tx buffer
    if (buffer_size(net_tx_buffer)) {
        dll_tx(buffer_pop(net_tx_buffer));
    }
    // check rx buffer
    if (buffer_size(net_rx_buffer)) {
        tran_rx(buffer_pop(net_rx_buffer));
    }
    // other functionality
    // ...
}
