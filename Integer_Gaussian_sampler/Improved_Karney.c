#include "sampler.h"

// //e^(-1/2*k^2)/(sum_k=0^infty e^(-1/2*k^2)): CDT
static const uint16_t expon_dist_13bit[] = { //5 elements for all, and each element in 16 bit (actually 15bit)

	// 14079u, 2744u, 214u, 6u //15bits
	// 3520u, 686u, 53u, 2u //13bit

	110u, 21u, 2u , 0u//8bit
}; //rcdt



//e^(-1/2*k^2)/(sum_k=0^infty e^(-1/2*k^2)): CDT
static const uint16_t expon_dist_8bit[] = { //5 elements for all, and each element in 16 bit (actually 15bit)

	// 14079u, 2744u, 214u, 6u //15bits
	// 3520u, 686u, 53u, 2u //13bit

	110u, 21u, 2u, 0u //8bit
	// 220u, 43u, 3u //9bit 
}; //rcdt




//For Fixed sigma = 1 and center = 0
static const uint16_t sampler2_rcocdt[] = {
    27u, 12u, 5u , 0u //6 bit
	// 14u, 6u, 3u //5bit, 不能通过
}; //rcdt



float generate_uniform_x(prng *p){
		
    int binary = 0x3f800000;
    int tmp = prng_get_u16(p);
	//    int tmp = (prng_get_u8(p)<<8) | prng_get_u8(p);
    tmp <<= 7;
    binary |= tmp;
	

    float x = *((float*)&binary) - 1;

    return x;
}



//rcdt
// int ks_sampler(prng *p,int *s, int *j){
// 	// uint32_t u;
// 	uint16_t w, u;
// 	int k = 0;
// 	u = prng_get_u16(p); 
// 	*s = u & 1;

	
// 	*j = (u >>1)&0x3ff; //10bit
// 	// u = (u>>3)&0x1fff; //13bit

// 	// u =  (u>>8)&0xfff; //13bit
// 	// u = (prng_get_u8(p)<<1) | (u>>15);  //9bit 

// 	u = prng_get_u8(p); 

// 	// printf("u = %d",u);
// 	// *j = u>>14;

// 	// printf("j = %d\n",*j);
// 	w = expon_dist_8bit[k]; //13bit
// 	// printf("u = %d, w = %d \n", u, w);
// 	while((u - w) >> 8){
// 		w = expon_dist_8bit[++k];
// 	}
// 	return k;
	
// }




//rcdt+randfloat: Fail
// int ks_sampler(prng *p,int *s, int *j, float *x){
// 	// uint32_t u;
// 	uint8_t w, u;
// 	int tmp = (prng_get_u16(p)<<8)|prng_get_u8(p), k = 0;
	
// 	*j = tmp &0x3ff; //10bit

	
// 	// u = (u>>3)&0x1fff; //13bit

// 	// u =  (u>>8)&0xfff; //13bit
// 	// u = (prng_get_u8(p)<<1) | (u>>15);  //9bit 

// 	u = prng_get_u8(p); 

// 	// printf("u = %d",u);
// 	// *j = u>>14;

// 	// printf("j = %d\n",*j);
// 	w = expon_dist_8bit[k]; //8bit
// 	// printf("u = %d, w = %d \n", u, w);
// 	while((u - w) >> 8){
// 		w = expon_dist_8bit[++k];
// 	}


// 	tmp = tmp>>10;
// 	*s = tmp & 1;

// 	//生成随机的浮点数
// 	int binary = 0x3f800000;
// 	tmp = (tmp>>1)<<10;
//     binary |= tmp;
//     *x = *((float*)&binary) - 1;

// 	return k;
// }



//rcocdt

int ks_sampler(prng *p,int *s, int *j, float *x){
	// uint32_t u;
	uint8_t w, u;
	int tmp = prng_get_u16(p), k = 0;
	
	*j = tmp &0x3ff; //10bit
	u = tmp>>10; 
	w = sampler2_rcocdt[k]; //6bit
	// printf("u = %d, w = %d \n", u, w);
	while((u - w) >> 6){
		w = sampler2_rcocdt[++k];
		u = prng_get_u8(p)>>2;
		// printf("k = %d, u = %d, w = %d \n", k, u, w);
	}

	tmp = prng_get_u16(p);

	*s = tmp & 1;

	//生成随机的浮点数
	int binary = 0x3f800000;
	tmp = (tmp>>1)<<8;
    binary |= tmp;
    *x = *((float*)&binary) - 1;

	return k;
}


//For center = 0 and sigma = 1024 is an integer
int Improved_Karney_for_Sampler2(prng *p) {
	int k, s, j;
	float random_float;
	while(1) {
		// STEP 1 Sample k from N(0,1) Half Discrete Gauss Sampler
		k = ks_sampler(p,&s,&j, &random_float);
		// k = ks_sampler(p,&s,&j);

		// STEP 2 
		// int j =  prng_get_u16(p)>>6; //random generate a 10 bit number
		// if ((j&k&s) == 0) continue;
		if(j == 0 && s == 0 && k == 0) continue;
		float x = j / 1024.;
		// STEP 3  
		// int h = k + 1;
		// while (h-- && AlgorithmB(p,k, x)) {}
		// if (!(h < 0)) continue; 
		// if(generate_uniform_x(p) > f_exp(-1./2 * x *(2.*k+ x))) continue;
		
		if(random_float > f_exp(-1./2 * x *(2.*k+ x))) continue;
		//STEP 4
		return (2*s-1) * ((k<<10) | j); 
	}
	return 0;
}

