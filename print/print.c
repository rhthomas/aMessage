#include "print.h"

void format(int i)
{
    if (!((i+1) % 8)) printf("\n");
    else printf("\t");
}

void print_struct(net_packet_t *p)
{
    printf("test struct\n");
    for (int i=0; i<p->length; i++) {
        printf("0x%02x", p->elem[i]);
        format(i);
    }
    printf("\r\n\n");
}

void print_array(uint8_t *a, uint8_t length)
{
    for (int i=0; i<length; i++) {
        printf("0x%02x", a[i]);
        format(i);
    }
    printf("\r\n\n");
}
