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
    3835u, 252u, 3u //13bit
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





//coef是a0,a1,a2,a3,a4......这种顺序存下去的
double Honer_algorithm(double x,double *coef,int length){
    double v=coef[length-1]*x+coef[length-2];
    for(int i=length-3;i>=0;i--){
        v=v*x+coef[i];
    }
    return v;
}
double f_exp(double x){ //直接默认x是负数
    double iexp=0.367879450321197510;
    double abs_x=-x;
    int integer_part=(int)abs_x;
    double remaining_part=abs_x-integer_part;

    //制表
    double exp_table[5];
    int exp_potent[5]={1,2,4,8,16};
    exp_table[0]=iexp;
    exp_table[1]=exp_table[0]*exp_table[0];
    exp_table[2]=exp_table[1]*exp_table[1];
    exp_table[3]=exp_table[2]*exp_table[2];
    exp_table[4]=exp_table[3]*exp_table[3];
    //依照快速幂算法计算整数部分,最多计算到e-32次方
    int binary_index[5];
    binary_index[4]=integer_part/16;
    if(binary_index[4]>=2) return 0; //溢出了，额外处理
    integer_part-=binary_index[4]*16;
    binary_index[3]=integer_part/8;
    integer_part-=binary_index[3]*8;
    binary_index[2]=integer_part/4;
    integer_part-=binary_index[2]*4;
    binary_index[1]=integer_part/2;
    integer_part-=binary_index[1]*2;
    binary_index[0]=integer_part;
    //计算主要部分
    double major=1;
    for(int i=0;i<5;i++){
        if(binary_index[i]==1)
            major*=exp_table[i];
    }

    //计算次要部分，由于次要部分x是小于1 的不妨计算x/2的级数展开，最后square回去
    double X=remaining_part/2.0;
    double root_minor=1;
    /*
    root_minor+=(-X);
    root_minor+=X*X/2;
    root_minor+=-X*X*X/6;
    root_minor+=X*X*X*X/24;
    root_minor+=-X*X*X*X*X/120;
    root_minor+=X*X*X*X*X*X/720;
    root_minor+=X*X*X*X*X*X*X/5040;
    */
    X=remaining_part/2.0;
    double coef[8]={1,1,1.0/2,1.0/6,1.0/24,1.0/120,1.0/720,1.0/5040};
    root_minor= Honer_algorithm(-X,coef,8);
    return root_minor*root_minor*major;
}


// float generate_uniform_x(prng *p){
		
//     int binary = 0x3f800000;
//     int tmp = prng_get_u16(p);
// 	//    int tmp = (prng_get_u8(p)<<8) | prng_get_u8(p);
//     tmp <<= 7;
//     binary |= tmp;
	

//     float x = *((float*)&binary) - 1;

//     return x;
// }


// float generate_uniform_random(prng *p, float x) { //���������[0, x]
//         return generate_uniform_x(p) * x;
// }


// //e^(-1/2*k^2)/(sum_k=0^infty e^(-1/2*k^2)): CDT
// static const uint16_t expon_dist_15bit[] = { //5 elements for all, and each element in 16 bit (actually 15bit)
// 	// 28158u, 5487u, 428u, 13u //uint_16
// 	14079u, 2744u, 214u, 6u //15bits
// 	// 19695u, 3838u, 299u, 9u
// }; //rcdt



// int ks_sampler(prng *p,int *s){
// 	uint16_t u , w;
// 	int k = 0;
// 	u = prng_get_u16(p); 
// 	*s = u & 1;
// 	u = u>>1;
// 	w = expon_dist_15bit[k]; //15bit
// 	// printf("u = %d, w = %d \n", u, w);
// 	while((u - w) >> 15){
// 		w = expon_dist_15bit[++k];
// 	}
// 	return k;
// }







// //For center = 0 and sigma = 1024 is an integer
// int Improved_Karney_for_Sampler2(prng *p) {
// 	int k, s;
// 	while(1) {
// 		// STEP 1 Sample k from N(0,1) Half Discrete Gauss Sampler
// 		k = ks_sampler(p,&s);
// 		// STEP 3 ȷ�Ϸ���

// 		// s = generate_uniform_sign(p); 
// 		// if(!s) s = -1;
		
// 		// STEP 4 ����С������ 
// 		// float di0 = stddev * k + s * mean;
// 		// int i0 = ceil(di0);
// 		// float x0 = (i0 - di0) / stddev;
// 		int j =  prng_get_u16(p)>>6; //random generate a 10 bit number
// 		// if ((j&k&s) == 0) continue;
// 		if(j == 0 && s == 0 && k == 0) continue;
// 		float x = j / 1024.;
// 		// STEP 5  
// 		// int h = k + 1;
// 		// while (h-- && AlgorithmB(p,k, x)) {}
// 		// if (!(h < 0)) continue; 
// 		if(generate_uniform_x(p) > exp(-1./2 * x *(2.*k+ x))) continue;
// 		//STEP 6 
// 		return (2*s-1) * ((k<<10) | j); 
// 	}
// 	return 0;
// }


// Fixed sigma = 0.75 and center = 0
int sampler_1(void *ctx){
    double sigma = 0.75;
    double center = 0;

    sampler_context *spc;
	spc = ctx; 
    int z = 0;
    /*
     * Get a random 16-bit value
     */
    uint16_t u = prng_get_u16(&spc->p);
    int b = u & 1;
    u = u>>3; //13bit
    for(; z < 3; z += 1){
        if((u - sampler1_rcdt[z]) >> 13 == 0){//If u<w, then (u - w) >> 12 > 0; Else, (u - w) >> 12 = 0
            return (b*2-1)*z;
        }
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
        if(generate_uniform_x(&spc->p) > f_exp(-x)) continue;
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
        if(generate_uniform_x(&spc->p) > f_exp(-x)) continue;
        // if(prob > exp(-x)) continue;
        return z1;
    }
}



