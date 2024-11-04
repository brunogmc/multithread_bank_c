#ifndef PCG_UTILS_H
#define PCG_UTILS_H

#include "pcg_variants.h"
#include <math.h>

extern pcg32_random_t pcg_rng;

void init_pcg_rng(void);
float pcg_random_float(void);
float gerar_valor_normal(float media, float desvio_padrao);

#endif