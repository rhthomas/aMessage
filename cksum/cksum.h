/**

    @file   cksum.h
    @author Rhys Thomas <rt8g15@soton.ac.uk>
    @date   2017-11-27T12:42

    @brief  Checksum functions for NET layer.

    TRAN and DLL layers use 8/16-bit CRC which will slow the processor down.
    Adding an extra CRC is unecessary since the data will be CRC'd twice anyway.
    At the NET layer we implement a simple XOR checksum, which does not entirely
    validate the integrity of the message, however further errors will be caught
    by either TRAN or DLL (based on the direction of the data).

*/

#ifndef CKSUM_H
#define CKSUM_H

#include <stdint.h>

#include "net.h"
#include "errors.h"

/**
    @brief  Calculate 8-bit XOR checksum.

    @param  data : Data to checksum.
    @param  length : Length of data.
    @return XOR sum of data.
*/
uint16_t xor_sum(net_packet_t *p);

/**
    @brief  Validate checksum.

    @param  data : Data to checksum.
    @param  length : Length of data.
    @retval ERROR_OK : Checksum passed.
    @retval ERROR_INV_CKSUM : Checksum invalid.
*/
error_t valid_cksum(net_packet_t *p);

#endif // CKSUM_H
