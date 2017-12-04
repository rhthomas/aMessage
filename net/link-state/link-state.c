/*

    Link-state routing functions.

*/

#include "link-state.h"

void shortest_path(uint8_t src, uint8_t dest, uint8_t path[])
{
    // state structure
    struct state {
        uint8_t prevNode;        // previous node in graph
        uint8_t length;          // length from source to this node
        enum {PERM, TENT} label; // label state
    } state[MAX_NODES];

    // initialise graph as infinite
    struct state *p;
    for (p = &state[0]; p < &state[MAX_NODES]; p++) {
        p->prevNode = -1;        // prevNode in optimal path from source
        p->length   = UNDEF_LEN; // unknown distance
        p->label    = TENT;
    }

    //  while k is not source
    state[t].length = 0;
    state[t].label  = PERM;
    uint8_t k = term; // initial working node

    while (k != src) {
        for (int i = 0; i < MAX_NODES; i++) {
            // if there is a distance and tentative state
            if (dist[k][i] != 0 && state[i].label == TENT) {
                uint8_t newLength = state[k].length + dist[k][i];
                if (newLength < state[i].length) {
                    state[i].prevNode = k;
                    state[i].length = newLength;
                }
            }
        }

        // find TENT node with smallels length
        k = 0; min = UNDEF_LEN;
        for (int i = 0; i < MAX_NODES; i++) {
            if (state[i].label == TENT && state[i].length < min) {
                min = state[i].length;
                k = i;
            }
        }
        state[k].label = PERM;
    }

    // copy path to output array (reverse list)
    k = src;
    while (k >= 0) {
        path[i++] = k;
        k = state[k].prevNode;
    }
}
