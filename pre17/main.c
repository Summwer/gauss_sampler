#include <stdio.h>
#include "sampler.h"
#include "speed_print.h"
#include "cpucycles.h"
#include <time.h>
// #include <memory_count.h>

int main(int argc, char *argv[]){
    int  output = atoi(argv[1]);
    FILE *file;
    clock_t start, finish;
	start = clock();

    sampler_shake256_context rng;
    sampler_context sc;
    fpr isigma , mu ; //, muinc; //= fpr_div(fpr_one, fpr_of(atoi(argv[1])));= fpr_of(atoi(argv[0]))

    
	if(output)
		file = fopen("output.txt", "w");  //输出到文件中 

    printf("Test sampler: \n");
    fflush(stdout);

    //生成随机数
    sampler_shake256_init(&rng); 
    sampler_shake256_inject(&rng, (const void *)"test sampler", 12);
    sampler_shake256_flip(&rng);
    Zf(prng_init)(&sc.p, &rng); 
    sc.sigma_min = fpr_sigma_min[9];

    //Here sigma = 1.7, center = -1
    isigma = fpr_div(fpr_of(10), fpr_of(17)); // isigma is equal to 1/sigma;The value of sigma MUST lie between 1 and 2 (i.e. isigma lies between * 0.5 and 1); in Falcon, sigma should always be between 1.2 and 1.9.
    mu = fpr_neg(fpr_one); //center, - 1
    // muinc = fpr_div(fpr_one, fpr_of(10)); 

    printf("pre17: Sample in N(%.3f, %.3f)\n", -1., 1.7);
    int z, NTESTS =0 ;



    // fprintf(file,"[");
	do{
		// t[i] = cpucycles();
		z = sampler(&sc, mu, isigma);
		NTESTS +=1;
		finish = clock();
        if(output)
			fprintf(file, "%d ", z);
	}while((finish - start)/CLOCKS_PER_SEC < 1.);

	printf("Generate %d samples in 1 seconds.\n",NTESTS);

    return 0;

}