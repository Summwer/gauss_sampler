#include <stdio.h>
#include "sampler.h"

int main() {
    sampler_shake256_context rng;
    sampler_context sc;
    fpr isigma, mu, muinc;

    printf("Test sampler: \n");
    fflush(stdout);


    //生成随机数
    sampler_shake256_init(&rng); 
    sampler_shake256_inject(&rng, (const void *)"test sampler", 12);
    sampler_shake256_flip(&rng);
    Zf(prng_init)(&sc.p, &rng); 
    sc.sigma_min = fpr_sigma_min[9];

    isigma = fpr_div(fpr_of(10), fpr_of(17)); // isigma is equal to 1/sigma;The value of sigma MUST lie between 1 and 2 (i.e. isigma lies between * 0.5 and 1); in Falcon, sigma should always be between 1.2 and 1.9.
    mu = fpr_neg(fpr_one); //center
    muinc = fpr_div(fpr_one, fpr_of(10));



    int z, sample_num = 100;
    printf("[");
    for(int i = 0; i< sample_num; i++){
        z = sampler(&sc, mu, isigma);
        if(i<sample_num - 1)
            printf("%d, ", z);
        else
            printf("%d", z);
    }
    printf("]\n");

    return 0;

}