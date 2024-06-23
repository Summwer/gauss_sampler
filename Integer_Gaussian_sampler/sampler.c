#include "sampler.h"
#include <math.h>
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


//For Fixed sigma = 0.75 and center = 0
static const uint16_t sampler1_rcdt[] = {
    1917u, 126u, 1u //12bit
    // 1252u, 82u, 1u //12bit, halfdiscretegauss
}; //rcdt

//For Fixed sigma = 1 and center = 0
// static const uint16_t sampler2_rcdt[] = {
//     19695u, 3838u, 299u, 9u //15bit
// }; //rcdt


//For Fixed sigma = 1.5 and center = 0
static const uint16_t sampler3_rcdt[] = {
    // 3007u, 1262u, 366u, 71u, 9u, 1u//13bit
    4750u, 1994u, 579u, 113u, 15u, 1u //13bit, halfgauss
}; //rcdt



static const uint8_t sampler3_rcocdt[] = {
    // 188u, 107u, 74u, 50u, 33u, 22u, 14u, 9u, 6u, 4u //8bit
    // 148u, 107u, 74u, 50u, 33u, 22u, 14u, 9u, 6u, 4u//halfgaussdist
    // 38000u, 27508u, 19022u, 12781u, 8437u, 5509u, 3573u, 2308u, 1487u, 956u, 614u, 394u, 253u, 162u, 104u, 67u, 43u, 27u, 18u, 11u //halfgaussdist

    148u, 107u, 74u, 50u, 33u, 22u, 14u, 9u, 6u, 4u, 2u
}; //rcdt



//For Fixed sigma = 1.6 and center = 0
static const uint16_t sampler4_rcdt[] = {
    4922u, 2232u, 735u, 172u, 28u, 3u //13bit, halfgauss
}; //rcdt




// Fixed sigma = 0.75 and center = 0
int sampler_1(void *ctx){
    double sigma = 0.75;
    double center = 0;

    sampler_context *spc;
	spc = ctx; 
    int z = 0, b;
    
    /*
     * Get a random 16-bit value
     */
    uint16_t u, w, lo; 
    w = sampler1_rcdt[z];  //15bit
    lo = prng_get_u16(&spc->p);
    // u = (lo>>1) & 0x7fff; //15bit
    u = lo>>4; //12bit
    b = lo & 1;

    while((u - w) >> 12){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
        w = sampler1_rcdt[++z];
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

    // return DiscreteGaussian_Karney(&spc->p,(int)center,(int)sigma); 

    return Improved_Karney_for_Sampler2(&spc->p); 
}

// Fixed sigma = 1.5 and center c is uniformly distributed over [0,1)
int sampler_3(void *ctx, double center){
    double sigma = 1.5;

    sampler_context *spc;
	spc = ctx; 
    // return DiscreteGaussian_Karney(&spc->p,center,sigma); 

    
    while(1){
        int z = 0, b;
        /*
        * Get a random 16-bit value
        */
        // uint8_t u, w, lo; 
        // w = sampler3_rcocdt[z];  //8bit
        // u = prng_get_u8(&spc->p);
        // b = prng_get_u8(&spc->p) & 1;

        // while((u - w) >> 8){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
        //     w = sampler3_rcocdt[++z];
        //     u = prng_get_u8(&spc->p);
        // }

        uint16_t u, w; 
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 3;

        while((u - w) >> 13){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
            w = sampler3_rcdt[++z];
        }
        
        
        int z1 = b + (2*b - 1) * z;
        double x = (z1-center)*(z1-center)/2./sigma/sigma -  z*z/2./sigma/sigma;
        // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
        if(generate_uniform_x(&spc->p) > exp(-x)) continue;
        // if(prob > exp(-x)) continue;
        return z1;
    }
}

// sigma is uniformly distributed over (0.8,1.6) and center is uniformly distributed over [0,1)
int sampler_4(void *ctx, double sigma, double center){

    // int z = 0;
    // return z;

    sampler_context *spc;
	spc = ctx; 
    // return DiscreteGaussian_Karney(&spc->p,center,sigma); 

    // return DiscreteGaussian_Karney(&spc->p,center,sigma); 
    while(1){
        int z = 0, b;
        /*
        * Get a random 16-bit value
        */
        // uint8_t u, w, lo; 
        // w = sampler3_rcocdt[z];  //8bit
        // u = prng_get_u8(&spc->p);
        // b = prng_get_u8(&spc->p) & 1;

        // while((u - w) >> 8){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
        //     w = sampler3_rcocdt[++z];
        //     u = prng_get_u8(&spc->p);
        // }

        uint16_t u, w; 
        w = sampler4_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 3;

        while((u - w) >> 13){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
            w = sampler4_rcdt[++z];
        }
        
        
        int z1 = b + (2*b - 1) * z;
        double x = (z1-center)*(z1-center)/2./sigma/sigma -  z*z/2./1.6/1.6;
        // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
        if(generate_uniform_x(&spc->p) > exp(-x)) continue;
        // if(prob > exp(-x)) continue;
        return z1;
    }
}



