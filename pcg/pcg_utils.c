#include "pcg_utils.h"
#include <time.h>

pcg32_random_t pcg_rng;

void init_pcg_rng(void) {
    pcg32_srandom_r(&pcg_rng, time(NULL), (intptr_t)&pcg_rng);
}

float pcg_random_float(void) {
    return ldexpf((float)pcg32_random_r(&pcg_rng), -32);
}

float gerar_valor_normal(float media, float desvio_padrao) {
    float u1 = pcg_random_float();
    float u2 = pcg_random_float();
    float z = sqrt(-2 * log(u1)) * cos(2 * M_PI * u2);
    return media + desvio_padrao * z;
}