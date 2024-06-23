#include "sampler.h"
#include <time.h>

void test_sampler(int testNum, bool output){

    //Initialize random generation. 
	sampler_shake256_context rng;
    sampler_context sc;
    sampler_shake256_init(&rng); 
    sampler_shake256_inject(&rng, (const void *)"test sampler", 12);
    sampler_shake256_flip(&rng);
    Zf(prng_init)(&sc.p, &rng); 


    /*
    Test Sampler.
    */
    printf("Test Sampler%d: \n", testNum);
    double sigma = 0 , center = 0;

    srand((unsigned)time(NULL));
    if(testNum == 3 )
        // Fixed sigma = 1.5 and center c is uniformly distributed over [0,1)
        center = ((double)rand() / RAND_MAX); 
        // center = 0;
    if(testNum == 4){
        // sigma is uniformly distributed over (0.8,1.6) and center is uniformly distributed over [0,1)
        sigma = ((double)rand() / RAND_MAX)*0.8 + 0.8;
        // sigma = ((double)rand() / RAND_MAX);
        center = ((double)rand() / RAND_MAX); 
    } 


	int i = 0, j = 0;
	FILE *file;
    // char *file_name = "output-%d.txt", testNum;
	if(output){
        char file_name[]= "output-0.txt";
        file_name[7]=testNum+'0'; 
		file = fopen(file_name, "w");  
        if(testNum == 1)
            fprintf(file, "0 0.75\n");
        if(testNum == 2)
            fprintf(file, "0 1024\n");
        if(testNum == 3)
            fprintf(file, "%.3f 1.5\n", center);
        if(testNum == 4)
            fprintf(file, "%.3f %.3f\n", center, sigma);
    }
	int NTESTS =0;
	int random_number;
   
    
    
    clock_t start, finish;
	start = clock();
    
	do{ 
        switch(testNum){
            case 1:
                random_number = sampler_1(&sc);
                break;
            case 2:
                random_number = sampler_2(&sc);
                break;
            case 3:
                random_number = sampler_3(&sc,center);
                break;
            case 4: 
                // sigma is uniformly distributed over (0.8,1.6) and center is uniformly distributed over [0,1)
                random_number = sampler_4(&sc,sigma,center);
                break;
            default:
                printf("Test Number should be in [1,4]\n");
                return;
        }
		NTESTS +=1;
		finish = clock();
        
		if(output)
			fprintf(file, "%d ", random_number);
	}while((finish - start)/CLOCKS_PER_SEC < 1);
	printf("Generate %d samples in 1 seconds.\n",NTESTS);

	if(output)
        fclose(file);

}

//input: mu, sigma 
int main() {
	// double mu = -1., sigma = 1.7;
	// double mu = -1, sigma = 1;

    // Pre-Setting: Output = 1, output the text; Else, output = 0
    bool output = true;
    root=(node *)malloc(sizeof (node));
    binary_p_mat=(uint32_t *) malloc((Pmat_row_size+1)*sizeof (uint32_t));
    expansion_margin=(node **)malloc(max_margin_size*sizeof (node *));
    for(int testNum = 1; testNum<=1;testNum++){
        test_sampler(testNum, output);
    }
	return 0;
}

