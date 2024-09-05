#include "sampler.h"
// #include <math.h>
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
    3835u, 252u, 3u,0u //13bit

    // 1917u, 126u, 2u, 1u //12bi   t
    // 1917u, 126u, 1u
    // 1252u, 82u, 1u //12bit, halfdiscretegauss
}; //rcdt





//For Fixed sigma = 1.5 and center = 0
static const uint16_t sampler3_rcdt[] = {
    // 4750u, 1994u, 579u, 113u, 15u, 1u //13bit, halfgauss
    19000u, 7975u, 2315u, 451u, 58u, 5u, 0u //15bit, halfgauss, 6 elements
}; //rcdt



//For Fixed sigma = 1.6 and center = 0
static const uint16_t sampler4_rcdt[] = {
    4922u, 2232u, 735u, 172u, 28u, 3u, 0u //13bit, halfgauss, 7elements
}; //rcdt




//For Fixed sigma = 1.5 and center = 0
static const uint8_t sampler3_rcocdt[] = {
    // 19u, 13u, 9u, 6u, 4u, 3u //5bit, halfgauss
    148u, 107u, 74u, 50u, 33u, 22u, 0u //8bit
    // 74u, 54u, 37u, 25u, 16u, 11u //7bit
}; //rcdt



static int ans_buffer[5];
static uint32_t bit_buffer=0;
static uint8_t bit_buffer_remaining=0;

static uint8_t sampler4_remaining=0;
//static int sampler4_ans_buffer[4];
static int sampler4_ans_buffer[8];

static uint8_t sampler3_remaining=0;
static int sampler3_ans_buffer[8];

static uint8_t sampler2_remaining=0;
static int sampler2_ans_buffer[8];
// Fixed sigma = 0.75 and center = 0



int sampler_1(void *ctx){
    //double sigma = 0.75;
    //double center = 0;
    sampler_context *spc;
	spc = ctx; 
    int z = 0;
    //Get a random 13-bit value

    uint16_t u = prng_get_u16(&spc->p);
    int b = u & 1;
    u =  u>>3; 
    for(; z < 3; z += 1){
        if((u - sampler1_rcdt[z]) >> 13 == 0){//If u<w, then (u - w) >> 12 > 0; Else, (u - w) >> 12 = 0
            // w = sampler1_rcdt[++z];
            break;
        }
    }

    return (b*2-1)*z;
}



int sampler_2(void *ctx){
    sampler_context *spc;
	spc = ctx; 
    static uint8_t saturated=0;
    int k[8] ,j[8],s[8] __attribute__((aligned(32)));
    float x[8] __attribute__((aligned(32)));
	float random_float[8]__attribute__((aligned(32)));
    float inv_1024=1/1024.0;
	while(1) {
        if(sampler2_remaining){
            return sampler2_ans_buffer[--sampler2_remaining];
        }
        if(saturated==8){
            __m256 x_vec=_mm256_setr_ps((float)j[0],(float)j[1],(float)j[2],(float)j[3],(float)j[4],(float)j[5],(float)j[6],(float)j[7]);
            x_vec=_mm256_mul_ps(x_vec,_mm256_set1_ps(inv_1024));
            __m256 random_float_vec=_mm256_load_ps(random_float);
            __m256 k_vec=_mm256_setr_ps((float)k[0],(float)k[1],(float)k[2],(float)k[3],(float)k[4],(float)k[5],(float)k[6],(float)k[7]);
            __m256 power=_mm256_fmadd_ps(_mm256_set1_ps(2.0),k_vec,x_vec);
            power=_mm256_mul_ps(power,x_vec);
            power=_mm256_mul_ps(power,_mm256_set1_ps(-0.5));
            power=exp256_ps(power);

            __m256 result=_mm256_cmp_ps(random_float_vec,power,_CMP_GT_OQ);
            float result_elements[8] __attribute__((aligned(32)));
            _mm256_store_ps(result_elements,result);

            for(int i=0;i<8;i++){
                if((int)result_elements[i]){
                    continue;
                }
                sampler2_ans_buffer[sampler2_remaining]=(2*s[i]-1) * ((k[i]<<10) | j[i]);
                sampler2_remaining++;
            }
            saturated=0;
        }
        else{
            k[saturated] = ks_sampler(&spc->p,&s[saturated],&j[saturated], &random_float[saturated]);
            if(j[saturated] == 0 && s[saturated] == 0 && k[saturated] == 0) continue;
            saturated++;
        }
	}
}


int sampler_3(void *ctx, double center){
    //double sigma = 1.5;
    int z = 0, b;
    int z_array[8];
    //float z_vec[8];
    sampler_context *spc;
	spc = ctx; 
    uint16_t u, w;
    // return DiscreteGaussian_Karney(&spc->p,center,sigma); 

    

    while(1){
        if(sampler3_remaining){
            sampler3_remaining--;
            return sampler3_ans_buffer[sampler3_remaining];
        }
        else{
        
        //it may seem redundant to keep 8 almost identical blocks of codes in this area.
        //We are reluctent to write it as a function which will be called 8 times during one execution of the loop.
        z=0;
        w = sampler3_rcdt[z];  //13bit
        int32_t y_array[8];
    

        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; 
        while((u - w) >> 15){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
            w = sampler3_rcdt[++z];
        }
        int z1 = b + (2*b - 1) * z;
        float x1 = (z1-center+z)*(center+z-z1) * isigma15;
        
        z_array[0]=z1;

        z=0;
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; 
        while((u - w) >> 15){
            w = sampler3_rcdt[++z];
        }
        int z2 = b + (2*b - 1) * z;
        float x2 = (z2-center+z)*(center+z-z2) * isigma15;
        z_array[1]=z2;

        z=0;
        w = sampler3_rcdt[z];  
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; 
        while((u - w) >> 15){
            w = sampler3_rcdt[++z];
        }
        int z3 = b + (2*b - 1) * z;
        float x3 = (z3-center+z)*(center+z-z3) * isigma15;
        z_array[2]=z3;

        z=0;
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1;
        while((u - w) >> 15){
            w = sampler3_rcdt[++z];
        }
        int z4 = b + (2*b - 1) * z;
        float x4 = (z4-center+z)*(center+z-z4) * isigma15;
        z_array[3]=z4;

        z=0;
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; 
        while((u - w) >> 15){
            w = sampler3_rcdt[++z];
        }
        int z5 = b + (2*b - 1) * z;
        float x5 = (z5-center+z)*(center+z-z5) * isigma15;
        z_array[4]=z5;

        z=0;
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; 
        while((u - w) >> 15){
            w = sampler3_rcdt[++z];
        }
        int z6 = b + (2*b - 1) * z;
        float x6 = (z6-center+z)*(center+z-z6) * isigma15;
        z_array[5]=z6;

        z=0;
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; 
        while((u - w) >> 15){
            w = sampler3_rcdt[++z];
        }
        int z7 = b + (2*b - 1) * z;
        float x7 = (z7-center+z)*(center+z-z7) * isigma15;
        z_array[6]=z7;

        z=0;
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; 
        while((u - w) >> 15){
            w = sampler3_rcdt[++z];
        }
        int z8 = b + (2*b - 1) * z;
        float x8 = (z8-center+z)*(center+z-z8) * isigma15;
        z_array[7]=z8;

        __m256 x=_mm256_setr_ps(x1,x2,x3,x4,x5,x6,x7,x8);
        __m256 exp_x=exp256_ps(x);

        prng_get_u128(&spc->p,y_array);
        __m256i y=_mm256_setr_epi32(y_array[0],y_array[1],y_array[2],y_array[3],y_array[4],y_array[5],y_array[6],y_array[7]);
        y=_mm256_slli_epi32(y,7);
        y=_mm256_or_si256(y,_mm256_set1_epi32(0x3f800000));
        __m256 y_vec=_mm256_castsi256_ps(y);
        y_vec=_mm256_sub_ps(y_vec,_mm256_set1_ps(1.0));


        //__m256 y=_mm256_setr_ps(y1,y2,y3,y4,y5,y6,y7,y8);
        __m256 result=_mm256_cmp_ps(y_vec,exp_x,_CMP_GT_OQ);
        float result_elements[8] __attribute__((aligned(32)));
        _mm256_store_ps(result_elements,result);
        for(int index =0;index<8;index++){
            if((int) result_elements[index]){
                continue;
            }
            sampler3_ans_buffer[sampler3_remaining]=z_array[index];
            sampler3_remaining++;
        }
        }
    }
}



int sampler_4(void *ctx, double sigma, double center){

    // int z = 0;
    // return z;

    sampler_context *spc;
	spc = ctx; 
    int z,b;
    int z_array[8];
    uint16_t u, w; 
    
    while(1){
        if(sampler4_remaining){
            sampler4_remaining--;
            return sampler4_ans_buffer[sampler4_remaining];
        }
        else{
            
            int32_t y_array[8];
            z=0;
            w = sampler4_rcdt[z];  //13bit
            u = prng_get_u16(&spc->p);
            b = u & 1;
            u = u >> 3;
            while((u - w) >> 13){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
                w = sampler4_rcdt[++z];
            }
            int z1 = b + (2*b - 1) * z;
            float x1 = (z1-center)*(z1-center)/2./sigma/sigma -  z*z*0.1953125; // 0.1953125 = 1/2./1.6/1.6
            z_array[0]=z1;
            
            z=0;
            w = sampler4_rcdt[z];  //13bit
            u = prng_get_u16(&spc->p);
            b = u & 1;
            u = u >> 3;
            while((u - w) >> 13){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
                w = sampler4_rcdt[++z];
            }
            int z2 = b + (2*b - 1) * z;
            float x2 = (z2-center)*(z2-center)/2./sigma/sigma -  z*z*0.1953125; // 0.1953125 = 1/2./1.6/1.6
            z_array[1]=z2;

            z=0;
            w = sampler4_rcdt[z];  //13bit
            u = prng_get_u16(&spc->p);
            b = u & 1;
            u = u >> 3;
            while((u - w) >> 13){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
                w = sampler4_rcdt[++z];
            }
            int z3 = b + (2*b - 1) * z;
            float x3 = (z3-center)*(z3-center)/2./sigma/sigma -  z*z*0.1953125; // 0.1953125 = 1/2./1.6/1.6
            z_array[2]=z3;

            z=0;
            w = sampler4_rcdt[z];  //13bit
            u = prng_get_u16(&spc->p);
            b = u & 1;
            u = u >> 3;
            while((u - w) >> 13){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
                w = sampler4_rcdt[++z];
            }
            int z4 = b + (2*b - 1) * z;
            float x4 = (z4-center)*(z4-center)/2./sigma/sigma -  z*z*0.1953125; // 0.1953125 = 1/2./1.6/1.6
            z_array[3]=z4;

            z=0;
            w = sampler4_rcdt[z];  //13bit
            u = prng_get_u16(&spc->p);
            b = u & 1;
            u = u >> 3;
            while((u - w) >> 13){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
                w = sampler4_rcdt[++z];
            }
            int z5 = b + (2*b - 1) * z;
            float x5 = (z5-center)*(z5-center)/2./sigma/sigma -  z*z*0.1953125; // 0.1953125 = 1/2./1.6/1.6
            z_array[4]=z5;

            z=0;
            w = sampler4_rcdt[z];  //13bit
            u = prng_get_u16(&spc->p);
            b = u & 1;
            u = u >> 3;
            while((u - w) >> 13){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
                w = sampler4_rcdt[++z];
            }
            int z6 = b + (2*b - 1) * z;
            float x6 = (z6-center)*(z6-center)/2./sigma/sigma -  z*z*0.1953125; // 0.1953125 = 1/2./1.6/1.6
            z_array[5]=z6;

            z=0;
            w = sampler4_rcdt[z];  //13bit
            u = prng_get_u16(&spc->p);
            b = u & 1;
            u = u >> 3;
            while((u - w) >> 13){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
                w = sampler4_rcdt[++z];
            }
            int z7 = b + (2*b - 1) * z;
            float x7 = (z7-center)*(z7-center)/2./sigma/sigma -  z*z*0.1953125; // 0.1953125 = 1/2./1.6/1.6
            z_array[6]=z7;

            z=0;
            w = sampler4_rcdt[z];  //13bit
            u = prng_get_u16(&spc->p);
            b = u & 1;
            u = u >> 3;
            while((u - w) >> 13){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
                w = sampler4_rcdt[++z];
            }
            int z8 = b + (2*b - 1) * z;
            float x8 = (z8-center)*(z8-center)/2./sigma/sigma -  z*z*0.1953125; // 0.1953125 = 1/2./1.6/1.6
            z_array[7]=z8;

            prng_get_u128(&spc->p,y_array);
            __m256i y=_mm256_setr_epi32(y_array[0],y_array[1],y_array[2],y_array[3],y_array[4],y_array[5],y_array[6],y_array[7]);
            y=_mm256_slli_epi32(y,7);
            y=_mm256_or_si256(y,_mm256_set1_epi32(0x3f800000));
            __m256 y_vec=_mm256_castsi256_ps(y);
            y_vec=_mm256_sub_ps(y_vec,_mm256_set1_ps(1.0));


        
            __m256 x=_mm256_setr_ps(-x1,-x2,-x3,-x4,-x5,-x6,-x7,-x8);
            __m256 exp_x=exp256_ps(x);
            __m256 result=_mm256_cmp_ps(y_vec,exp_x,_CMP_GT_OQ);

            float result_elements[8] __attribute__((aligned(32)));
            _mm256_store_ps(result_elements,result);
            for(int index=0;index<8;index++){
                if((int)result_elements[index]){
                    //greater than
                    continue;
                }
                sampler4_ans_buffer[sampler4_remaining]=z_array[index];
                sampler4_remaining++;
            }
        }
    }
}