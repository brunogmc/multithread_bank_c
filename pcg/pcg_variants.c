#include "pcg_variants.h"

static inline uint32_t pcg_output_xsh_rr_64_32(uint64_t state) {
    return (uint32_t)(((state >> 18u) ^ state) >> ((state >> 59u) & 0x1fu));
}

void pcg32_srandom_r(pcg32_random_t* rng, uint64_t initial_state, uint64_t seq) {
    rng->state = 0U;
    rng->inc = (seq << 1u) | 1u;
    pcg32_random_r(rng);
    rng->state += initial_state;
    pcg32_random_r(rng);
}

uint32_t pcg32_random_r(pcg32_random_t* rng) {
    uint64_t oldstate = rng->state;
    rng->state = oldstate * 6364136223846793005ULL + rng->inc;
    return pcg_output_xsh_rr_64_32(oldstate);
}