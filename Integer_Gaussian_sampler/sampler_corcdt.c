#include "sampler.h"

/*
 *
 * ********************************Attention!!!************************************************
 * ctx : Pointer to the sampler_context structure, including PRNG or other.
 * ********************************************************************************************
 * Please read the following precautions carefully!!!!
 * 1. In supplied folder "example/sampler", it offers a falcon's sampler as an instance for giving the codes about pointer "ctx".
 * 2. In folder "example/sampler", it includes the implementation of SHAKE256 in shake.c and PRNG in rng.c, please just use it.
 * 3. In the head file "example/sampler/sampler.h", it gives the definition of "ctx" in the struct "sampler_context", please just use it and don't change it.
 * 4. In the main file "example/sampler/main.c", it provides the procedure of how to initialize the pointer ctx, you should learn it and use it.
 * 5. The API of sampler function in the "example/sampler" is similar to ours. Pointer ctx is the same and fpr also denotes the type of "double", but we use "double" here.
 *
 *
 *In conclusion, you should use the method of defining and initializing for pointer ctx in given "example/sampler",
 *please don't change it and just use it. You should focus on the four samplers themselves. Pointer ctx should be the same for everyone.
 *
 *
 * */


static const uint8_t sampler1_corcdt[] = {
    // 60u, 8u, 1u //7bit for Fixed sigma = 0.75 and center = 0
    120u, 17u, 3u, 1u //8bit for Fixed sigma = 0.75 and center = 0
}; //rcdt

// Fixed sigma = 0.75 and center = 0
int sampler_1(void *ctx){
    double sigma = 0.75;
    double center = 0;

    sampler_context *spc;
	spc = ctx; 
    int z = 0, b;
    
    /*
     * Get a random 8-bit value
     */
    uint8_t u, w, lo; 
    w = sampler1_corcdt[z] ;  //8bit
    lo = (uint8_t) prng_get_u8(&spc->p);
    u =  prng_get_u8(&spc->p); //(lo>>1) & 0x7f; //7bit
    b = prng_get_u8(&spc->p) & 1;
    // b = lo & 1;
    // u =  (lo>>1)& 0x7f; //7bit

    // while(u <= w){
    while((u - w) >> 8){//If u<w, then (u - w) >> 7 > 0; Else, (u - w) >> 7 = 0
        w =sampler1_corcdt[++z];
        u = (prng_get_u8(&spc->p));
        // u = ((uint8_t) prng_get_u8(&spc->p)>>1); //7bit
        // printf("w= %u, u = %u, (u - w) >> 7 = %u\n" ,w,u,(u - w) >> 7);
    }
    return (b*2-1)*z;
}



// Fixed sigma = 1024 and center = 0
int sampler_2(void *ctx){
    double sigma = 1024;
    double center = 0;

    // int z = 0;
    sampler_context *spc;
	spc = ctx; 

    return DiscreteGaussian_Karney(&spc->p,center,sigma); 

    // return z;

}

// Fixed sigma = 1.5 and center c is uniformly distributed over [0,1)
int sampler_3(void *ctx, double center){
    double sigma = 1.5;

    // int z = 0;


    // return z;

    sampler_context *spc;
	spc = ctx; 
    return DiscreteGaussian_Karney(&spc->p,center,sigma); 



}

// sigma is uniformly distributed over (0.8,1.6) and center is uniformly distributed over [0,1)
int sampler_4(void *ctx, double sigma, double center){

    // int z = 0;
    // return z;

    sampler_context *spc;
	spc = ctx; 
    return DiscreteGaussian_Karney(&spc->p,center,sigma); 
}



