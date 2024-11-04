#ifndef PCG_VARIANTS_H
#define PCG_VARIANTS_H

#include <stdint.h>

typedef struct {
    uint64_t state;
    uint64_t inc;
} pcg32_random_t;

void pcg32_srandom_r(pcg32_random_t* rng, uint64_t initial_state, uint64_t seq);
uint32_t pcg32_random_r(pcg32_random_t* rng);
void pcg32_advance_r(pcg32_random_t* rng, uint64_t delta);

#endif