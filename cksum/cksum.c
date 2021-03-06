#include "cksum.h"

uint16_t xor_sum(net_packet_t *p)
{
    uint16_t sum = 0;
    for (int byte = 0; byte < p->length-2; byte++) {
        sum ^= p->elem[byte];
    }
    return sum;
}

error_t valid_cksum(net_packet_t *p)
{
    if (xor_sum(p) == p->cksum) {
        return ERROR_OK;
    } else {
        return ERROR_INV_CKSUM;
    }
}
