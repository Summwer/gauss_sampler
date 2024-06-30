#include "sampler.h"

//e^(-1/2*k^2)/(sum_k=0^infty e^(-1/2*k^2)): CDT
static const uint16_t expon_dist_15bit[] = { //5 elements for all, and each element in 16 bit (actually 15bit)
	// 28158u, 5487u, 428u, 13u //uint_16
	14079u, 2744u, 214u, 6u //15bits
	// 19695u, 3838u, 299u, 9u
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

int ks_sampler(prng *p,int *s){
	uint16_t u , w;
	int k = 0;
	u = prng_get_u16(p); 
	*s = u & 1;
	u = u>>1;
	w = expon_dist_15bit[k]; //15bit
	// printf("u = %d, w = %d \n", u, w);
	while((u - w) >> 15){
		w = expon_dist_15bit[++k];
	}
	return k;
}




//For center = 0 and sigma = 1024 is an integer
int Improved_Karney_for_Sampler2(prng *p) {
	int k, s;
	while(1) {
		// STEP 1 Sample k from N(0,1) Half Discrete Gauss Sampler
		k = ks_sampler(p,&s);

		// STEP 2 
		int j =  prng_get_u16(p)>>6; //random generate a 10 bit number
		// if ((j&k&s) == 0) continue;
		if(j == 0 && s == 0 && k == 0) continue;
		float x = j / 1024.;
		// STEP 3  
		// int h = k + 1;
		// while (h-- && AlgorithmB(p,k, x)) {}
		// if (!(h < 0)) continue; 
		if(generate_uniform_x(p) > f_exp(-1./2 * x *(2.*k+ x))) continue;
		//STEP 4
		return (2*s-1) * ((k<<10) | j); 
	}
	return 0;
}

