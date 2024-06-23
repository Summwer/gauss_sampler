#include <stdio.h>
#include "sampler.h"

int main() {
    sampler_shake256_context rng;
    sampler_context sc;
    fpr isigma, mu, muinc;

    printf("Test sampler: \n");
    fflush(stdout);

    sampler_shake256_init(&rng);
    sampler_shake256_inject(&rng, (const void *)"test sampler", 12);
    sampler_shake256_flip(&rng);
    Zf(prng_init)(&sc.p, &rng);
    sc.sigma_min = fpr_sigma_min[9];

    isigma = fpr_div(fpr_of(10), fpr_of(17));
    mu = fpr_neg(fpr_one);
    muinc = fpr_div(fpr_one, fpr_of(10));

    int z;

    z = sampler(&sc, mu, isigma);

    printf("z: %d", z);

    return 0;

}