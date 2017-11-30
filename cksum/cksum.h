/**

    @file   cksum.h
    @author Rhys Thomas <rt8g15@soton.ac.uk>
    @date   2017-11-27T12:42

    @brief  Checksum functions for NET layer.

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
uint8_t xor_sum(net_packet_t *p);

/**
    @brief  Validate checksum.

    @param  data : Data to checksum.
    @param  length : Length of data.
    @retval ERROR_OK : Checksum passed.
    @retval ERROR_INV_CKSUM : Checksum invalid.
*/
error_t valid_cksum(net_packet_t *p);

#endif // CKSUM_H
