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
    // 1917u, 126u, 2u, 1u
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
    u =  u>>3; // u>>3; //13bit
    // for(; z < 3; z += 1){
    //     if((u - sampler1_rcdt[z]) >> 13 == 0){//If u<w, then (u - w) >> 12 > 0; Else, (u - w) >> 12 = 0

    for(; z < 3; z += 1){
        if((u - sampler1_rcdt[z]) >> 13 == 0){//If u<w, then (u - w) >> 12 > 0; Else, (u - w) >> 12 = 0
            // w = sampler1_rcdt[++z];
            break;
        }
    }

    // uint16_t w = sampler1_rcdt[z];
    // while((u - w) >> 13){
    //     w = sampler1_rcdt[++z];
    // }
    return (b*2-1)*z;
}


/*
int sampler_1(void *ctx){
    static int count=0;
    static int round_num=0;
    round_num++;
    sampler_context *spc;
	spc = ctx; 
    
    //设置一个buffer，一次性算好很多个，然后吐给调用者
    
    if(count==0){
        //重新计算好buffer
        uint64_t u1 = prng_get_u64(&spc->p); //可以直接利用这64bit计算出四个结果,第五个结果根据saturated来判断是否计算
        int b=u1&1;
        uint64_t mask=0x0000000000003ffeu; //取13bit
        uint16_t u=(u1&mask)>>1;
        u1>>=14; //每次剩下两个bit
        int z=0;
        for(;z<3;z+=1){
            if((u - sampler1_rcdt[z]) >> 13 == 0){//If u<w, then (u - w) >> 12 > 0; Else, (u - w) >> 12 = 0
                break;
            }
        }
        ans_buffer[count]=(b*2-1)*z;
        count++;

        b=u1&1;
        u=(u1&mask)>>1;
        u1>>=14; //每次剩下两个bit
        z=0;
        for(;z<3;z+=1){
            if((u - sampler1_rcdt[z]) >> 13 == 0){//If u<w, then (u - w) >> 12 > 0; Else, (u - w) >> 12 = 0
                break;
            }
        }
        ans_buffer[count]=(b*2-1)*z;
        count++;

        b=u1&1;
        u=(u1&mask)>>1;
        u1>>=14; //每次剩下两个bit
        z=0;
        for(;z<3;z+=1){
            if((u - sampler1_rcdt[z]) >> 13 == 0){//If u<w, then (u - w) >> 12 > 0; Else, (u - w) >> 12 = 0
                break;
            }
        }
        ans_buffer[count]=(b*2-1)*z;
        count++;

        b=u1&1;
        u=(u1&mask)>>1;
        u1>>=14; //每次剩下两个bit
        z=0;
        for(;z<3;z+=1){
            if((u - sampler1_rcdt[z]) >> 13 == 0){//If u<w, then (u - w) >> 12 > 0; Else, (u - w) >> 12 = 0
                break;
            }
        }
        ans_buffer[count]=(b*2-1)*z;
        count++;
        
        //8bit remains in u1
        bit_buffer<<=8;
        bit_buffer_remaining+=8;
        bit_buffer|=(uint32_t)u1;
        if(bit_buffer_remaining>=14&&count<5){
            b=bit_buffer&1;
            u=(bit_buffer&(uint32_t)0x00003ffeu)>>1;
            bit_buffer>>=14;
            bit_buffer_remaining-=14;
            z=0;
            for(;z<3;z+=1){
                if((u - sampler1_rcdt[z]) >> 13 == 0){//If u<w, then (u - w) >> 12 > 0; Else, (u - w) >> 12 = 0
                break;
                }
            }
            ans_buffer[count]=(b*2-1)*z;
            count++;
        }
    }
    count--;
    return ans_buffer[count];
}
*/
// Fixed sigma = 1024 and center = 0

/*
int sampler_2(void *ctx){
    double sigma = 1024;
    double center = 0;

    // int z = 0;
    sampler_context *spc;
	spc = ctx; 

    // return DiscreteGaussian_Karney(&spc->p,(int)center,(int)sigma); 

    return Improved_Karney_for_Sampler2(&spc->p); 
}
*/

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
        //执行8轮
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

// Fixed sigma = 1.5 and center c is uniformly distributed over [0,1)
// rcdt

/*
int sampler_3(void *ctx, double center){
    double sigma = 1.5;

    sampler_context *spc;
	spc = ctx; 
    // return DiscreteGaussian_Karney(&spc->p,center,sigma); 

    while(1){
        int z = 0, b;
        
        //Get a random 15-bit value
        

        uint16_t u, w; 
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; //15bit

        while((u - w) >> 15){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
            w = sampler3_rcdt[++z];
        }
        
        
        int z1 = b + (2*b - 1) * z;
        double x = (z1-center+z)*(center+z-z1) * isigma15;
        // double x = (z1-center)*(z1-center)/2./sigma/sigma -  z*z/2./sigma/sigma;
        // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
        if(generate_uniform_x(&spc->p) > f_exp(x)) continue;
        // if(prob > exp(-x)) continue;
        return z1;
    }
}
*/




// Fixed sigma = 1.5 and center c is uniformly distributed over [0,1)
// rcocdt
/*
int sampler_3(void *ctx, double center){
    double sigma = 1.5;

    sampler_context *spc;
	spc = ctx; 
    // return DiscreteGaussian_Karney(&spc->p,center,sigma); 

    while(1){
        int z = 0, b;
        
        //Get a random 13-bit value
        

        uint16_t u, w; 
        w = sampler3_rcocdt[z];  //5bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 9; //5bit

        // printf("u = %d, w = %d\n", u, w);
        while((u - w) >> 7){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
            w = sampler3_rcocdt[++z];
            u = prng_get_u16(&spc->p);
            b = u & 1;
            u = u >> 9; //5bit
        }
        
        
        int z1 = b + (2*b - 1) * z;
        double x = (z1-center+z)*(center+z-z1) * isigma15;
        // double x = (z1-center)*(z1-center)/2./sigma/sigma -  z*z/2./sigma/sigma;
        // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
        if(generate_uniform_x(&spc->p) > f_exp(x)) continue;
        // if(prob > exp(-x)) continue;
        return z1;
    }
}
*/



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
        
        
        z=0;
        w = sampler3_rcdt[z];  //13bit
        int32_t y_array[8];
    

        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; //把这两个bit拿出来还能接着用
        while((u - w) >> 15){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
            w = sampler3_rcdt[++z];
        }
        int z1 = b + (2*b - 1) * z;
        float x1 = (z1-center+z)*(center+z-z1) * isigma15;
        // double x = (z1-center)*(z1-center)/2./sigma/sigma -  z*z/2./sigma/sigma;
        // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
        //float y1 = generate_uniform_x(&spc->p);
        z_array[0]=z1;
        //z_vec[0]=z;

        z=0;
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; //把这两个bit拿出来还能接着用
        while((u - w) >> 15){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
        // for(; z < 3; z += 1){
            w = sampler3_rcdt[++z];
        }
        int z2 = b + (2*b - 1) * z;
        float x2 = (z2-center+z)*(center+z-z2) * isigma15;
        // double x = (z1-center)*(z1-center)/2./sigma/sigma -  z*z/2./sigma/sigma;
        // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
        //float y2 = generate_uniform_x(&spc->p);
        z_array[1]=z2;
        //z_vec[1]=z;

        z=0;
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; //把这两个bit拿出来还能接着用
        while((u - w) >> 15){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
            w = sampler3_rcdt[++z];
        }
        int z3 = b + (2*b - 1) * z;
        float x3 = (z3-center+z)*(center+z-z3) * isigma15;
        // double x = (z1-center)*(z1-center)/2./sigma/sigma -  z*z/2./sigma/sigma;
        // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
        //float y3 = generate_uniform_x(&spc->p);
        z_array[2]=z3;
        //z_vec[2]=z;

        z=0;
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; //把这两个bit拿出来还能接着用
        while((u - w) >> 15){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
            w = sampler3_rcdt[++z];
        }
        int z4 = b + (2*b - 1) * z;
        float x4 = (z4-center+z)*(center+z-z4) * isigma15;
        // double x = (z1-center)*(z1-center)/2./sigma/sigma -  z*z/2./sigma/sigma;
        // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
        //float y4 = generate_uniform_x(&spc->p);
        z_array[3]=z4;
        //z_vec[3]=z;

        z=0;
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; //把这两个bit拿出来还能接着用
        while((u - w) >> 15){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
            w = sampler3_rcdt[++z];
        }
        int z5 = b + (2*b - 1) * z;
        float x5 = (z5-center+z)*(center+z-z5) * isigma15;
        // double x = (z1-center)*(z1-center)/2./sigma/sigma -  z*z/2./sigma/sigma;
        // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
        //float y5 = generate_uniform_x(&spc->p);
        z_array[4]=z5;
        //z_vec[4]=z;

        z=0;
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; //把这两个bit拿出来还能接着用
        while((u - w) >> 15){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
            w = sampler3_rcdt[++z];
        }
        int z6 = b + (2*b - 1) * z;
        float x6 = (z6-center+z)*(center+z-z6) * isigma15;
        // double x = (z1-center)*(z1-center)/2./sigma/sigma -  z*z/2./sigma/sigma;
        // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
        //float y6 = generate_uniform_x(&spc->p);
        z_array[5]=z6;
        //z_vec[5]=z;

        z=0;
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; //把这两个bit拿出来还能接着用
        while((u - w) >> 15){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
            w = sampler3_rcdt[++z];
        }
        int z7 = b + (2*b - 1) * z;
        float x7 = (z7-center+z)*(center+z-z7) * isigma15;
        // double x = (z1-center)*(z1-center)/2./sigma/sigma -  z*z/2./sigma/sigma;
        // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
        //float y7 = generate_uniform_x(&spc->p);
        z_array[6]=z7;
        //z_vec[6]=z;

        z=0;
        w = sampler3_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 1; //把这两个bit拿出来还能接着用
        while((u - w) >> 15){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
            w = sampler3_rcdt[++z];
        }
        int z8 = b + (2*b - 1) * z;
        float x8 = (z8-center+z)*(center+z-z8) * isigma15;
        // double x = (z1-center)*(z1-center)/2./sigma/sigma -  z*z/2./sigma/sigma;
        // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
        //float y8 = generate_uniform_x(&spc->p);
        z_array[7]=z8;
        //z_vec[7]=z;

        /*
        __m256 center_array=_mm256_set1_ps((float)center);
        __m256 z_num_array=_mm256_setr_ps(z1,z2,z3,z4,z5,z6,z7,z8);
        __m256 z_vec=_mm256_setr_ps(z_vec[0],z_vec[1],z_vec[2],z_vec[3],z_vec[4],z_vec[5],z_vec[6],z_vec[7]);
        __m256 isigma15_array=_mm256_set1_ps((float)isigma15);
        __m256 first_part=_mm256_add_ps(z_num_array,z_vec);
        first_part=_mm256_sub_ps(first_part,center_array);

        __m256 second_part=_mm256_add_ps(center_array,z_vec);
        second_part=_mm256_sub_ps(second_part,z_num_array);
        __m256 x=_mm256_mul_ps(first_part,second_part);
        x=_mm256_mul_ps(x,isigma15_array);
        
        */
        
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



/*
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
        uint16_t u, w; 
        w = sampler4_rcdt[z];  //13bit
        u = prng_get_u16(&spc->p);
        b = u & 1;
        u = u >> 3;

        while((u - w) >> 13){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
            w = sampler4_rcdt[++z];
        }
        
        
        int z1 = b + (2*b - 1) * z;
        double x = (z1-center)*(z1-center)/2./sigma/sigma -  z*z*0.1953125; // 0.1953125 = 1/2./1.6/1.6
        // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
        if(generate_uniform_x(&spc->p) > f_exp(-x)) continue;
        // if(prob > exp(-x)) continue;
        return z1;
    }
}
*/

/*
//减小exp的计算，最好四个四个一起计算
int sampler_4(void *ctx, double sigma, double center){

    // int z = 0;
    // return z;

    sampler_context *spc;
	spc = ctx; 
    // return DiscreteGaussian_Karney(&spc->p,center,sigma); 
    int z,b;
    int z_array[4];
    uint16_t u, w; 
    // return DiscreteGaussian_Karney(&spc->p,center,sigma); 
    while(1){
        
        if(sampler4_remaining){
            sampler4_remaining--;
            return sampler4_ans_buffer[sampler4_remaining];
        }
        else{
            //每一轮计算四个
            z=0;
            w = sampler4_rcdt[z];  //13bit
            u = prng_get_u16(&spc->p);
            b = u & 1;
            u = u >> 3;
            while((u - w) >> 13){//If u<w, then (u - w) >> 16 > 0; Else, (u - w) >> 16 = 0
                w = sampler4_rcdt[++z];
            }
            int z1 = b + (2*b - 1) * z;
            double x1 = (z1-center)*(z1-center)/2./sigma/sigma -  z*z*0.1953125; // 0.1953125 = 1/2./1.6/1.6
            // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
            double y1 = generate_uniform_x(&spc->p);
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
            double x2 = (z2-center)*(z2-center)/2./sigma/sigma -  z*z*0.1953125; // 0.1953125 = 1/2./1.6/1.6
            // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
            double y2 = generate_uniform_x(&spc->p);
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
            double x3 = (z3-center)*(z3-center)/2./sigma/sigma -  z*z*0.1953125; // 0.1953125 = 1/2./1.6/1.6
            // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
            double y3 = generate_uniform_x(&spc->p);
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
            double x4 = (z4-center)*(z4-center)/2./sigma/sigma -  z*z*0.1953125; // 0.1953125 = 1/2./1.6/1.6
            // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
            double y4 = generate_uniform_x(&spc->p);
            z_array[3]=z4;
            

            __m256d y=_mm256_setr_pd(y1,y2,y3,y4);
            __m256d x=_mm256_setr_pd(-x1,-x2,-x3,-x4);
            //计算x的指数并且与y对比
            __m256d exp_x=exp256_pd(x);
            __m256d result=_mm256_cmp_pd(y,exp_x,_CMP_GT_OQ);
            double result_elements[4] __attribute__((aligned(32)));
            _mm256_store_pd(result_elements,result);
            for(int index=0;index<4;index++){
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
*/

int sampler_4(void *ctx, double sigma, double center){

    // int z = 0;
    // return z;

    sampler_context *spc;
	spc = ctx; 
    // return DiscreteGaussian_Karney(&spc->p,center,sigma); 
    int z,b;
    int z_array[8];
    uint16_t u, w; 
    // return DiscreteGaussian_Karney(&spc->p,center,sigma); 
    while(1){
        if(sampler4_remaining){
            sampler4_remaining--;
            return sampler4_ans_buffer[sampler4_remaining];
        }
        else{
            //每一轮计算8个
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
            // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
            //float y1 = generate_uniform_x(&spc->p);
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
            // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
            //float y2 = generate_uniform_x(&spc->p);
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
            // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
            //float y3 = generate_uniform_x(&spc->p);
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
            // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
            //float y4 = generate_uniform_x(&spc->p);
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
            // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
            //float y5 = generate_uniform_x(&spc->p);
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
            // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
            //float y6 = generate_uniform_x(&spc->p);
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
            // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
            //float y7 = generate_uniform_x(&spc->p);
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
            // double prob = (double)((prng_get_u16(&spc->p)<<16)| prng_get_u16(&spc->p)) / (double) (2<<32);
            //float y8 = generate_uniform_x(&spc->p);
            z_array[7]=z8;

            prng_get_u128(&spc->p,y_array);
            __m256i y=_mm256_setr_epi32(y_array[0],y_array[1],y_array[2],y_array[3],y_array[4],y_array[5],y_array[6],y_array[7]);
            y=_mm256_slli_epi32(y,7);
            y=_mm256_or_si256(y,_mm256_set1_epi32(0x3f800000));
            __m256 y_vec=_mm256_castsi256_ps(y);
            y_vec=_mm256_sub_ps(y_vec,_mm256_set1_ps(1.0));


            //__m256 y=_mm256_setr_ps(y1,y2,y3,y4,y5,y6,y7,y8);
            __m256 x=_mm256_setr_ps(-x1,-x2,-x3,-x4,-x5,-x6,-x7,-x8);
            //计算x的指数并且与y对比
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