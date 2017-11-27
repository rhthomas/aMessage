#include "net.h"
#include "cksum.h"
#include "print.h"

int main()
{
    uint8_t inc_data[] = {
        0x1c, 0x00, 0xaa, 0xab,
        0x00, 0x00, 0x11, 0x22,
        0x33, 0x44, 0x55, 0x66,
        0x77, 0x00, 0x00
    };

    net_packet_t p = net_to_struct(inc_data, sizeof(inc_data));

    // get size
    p.length = sizeof(inc_data);
    printf("size: %d\n", p.length);

    // get checksum
    p.cksum  = xor_sum(&p);
    printf("cksum: 0x%04x\n", p.cksum);

    // test checksum
    error_t err = 0;
    if (!(err == valid_cksum(&p))) {
        printf("ok\n");
    } else {
        printf("error\n");
    }

    return 0;
}

// 0x1c; 0x00; 0xaa; 0xab; 0x00; 0x00; 0x11; 0x22; 0x33; 0x44; 0x55; 0x66; 0x77; 0x00; 0x00
