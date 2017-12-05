#include "print.h"

void format(int i)
{
    if (!((i+1) % 8)) printf("\n");
    else printf("\t");
}

void print_struct(net_packet_t p)
{
    for (uint8_t i=0; i<128; i++) {
        printf("0x%02x", p.elem[i]);
        // skip any unused TRAN data
        if (i == p.length-3) {
            // jump to cksum field
            i = 125;
        }
        format(i);
    }
    printf("\r\n\n");
}

void print_array(uint8_t *a, uint8_t length)
{
    for (uint8_t i=0; i<length; i++) {
        printf("0x%02x", a[i]);
        // skip any unused TRAN data
        if (i == a[4]-3) {
            // jump to cksum field
            i = 125;
        }
        format(i);
    }
    printf("\r\n\n");
}
