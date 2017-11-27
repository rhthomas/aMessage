#ifndef PRINT_H
#define PRINT_H

#include <stdio.h>
#include <stdint.h>

#include "net.h"

void format(int i);

void print_struct(net_packet_t *p);

void print_array(uint8_t *a, uint8_t length);

#endif // PRINT_H
