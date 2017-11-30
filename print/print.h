/**

    Print functions for testbenches.

*/

#ifndef PRINT_H
#define PRINT_H

#include <stdio.h>
#include <stdint.h>

#include "net.h"

/**
    Format byte arrays into a grid.
*/
void format(int i);

/**
    Print structures.
*/
void print_struct(const net_packet_t p);

/**
    Print byte arrays.
*/
void print_array(uint8_t *a, uint8_t length);

#endif // PRINT_H
