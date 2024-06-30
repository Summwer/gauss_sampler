#include "sampler.h"
// #include <math.h>


float generate_uniform_x(prng *p){
		
    int binary = 0x3f800000;
    int tmp = prng_get_u16(p);
	//    int tmp = (prng_get_u8(p)<<8) | prng_get_u8(p);
    tmp <<= 7;
    binary |= tmp;
	

    float x = *((float*)&binary) - 1;

    return x;
}

// float generate_uniform_x(prng *p) { //生成随机数[0, x]
// 	return prng_get_u16(p) / 0xffff;
// }


// double generate_uniform_random(prng *p, double x) { //生成随机数[0, x]
// 	return ((double)prng_get_u8(p) / 0x100) * x;
// }



float generate_uniform_random(prng *p, float x) { //���������[0, x]
        return generate_uniform_x(p) * x;
}


int generate_uniform_sign(prng *p){
	return prng_get_u8(p) & 1;
}

//delete
/*
bool AlgorithmHDouble(prng *p) {
	float h_a, h_b;
	h_a = generate_uniform_x(p);
	// less than the half
	if (!(h_a < 0.5)) return true; 
	for (;;) {
		h_b = generate_uniform_x(p);
		if (!(h_b < h_a))
			return false;
		else
			h_a = generate_uniform_x(p);
		if (!(h_a < h_b)) return true;
	}
}
*/




// It returns 1 with probability e^(-1/2)=0.6065306597126334, returns 0 with probability 1-e^(-1/2). 
bool AlgorithmH(prng *p){
	float u = generate_uniform_x(p); //It's a float between (0,1)
	if( u <= 0.6065306597126334)
		return true;
	else
		return false;
}



//e^(-1/2*k^2)/(sum_k=0^infty e^(-1/2*k^2)): CDT
static const uint16_t expon_dist_15bit[] = { //5 elements for all, and each element in 16 bit (actually 15bit)
	// 28158u, 5487u, 428u, 13u //uint_16
	14079u, 2744u, 214u, 6u //15bits
	// 19695u, 3838u, 299u, 9u
}; //rcdt

//e^(-1/2*k^2)/(sum_k=0^infty e^(-1/2*k^2)): CDT
static const uint16_t expon_dist_16bit[] = { //5 elements for all, and each element in 16 bit (actually 15bit)
	28158u, 5487u, 428u, 13u //uint_16
}; //rcdt


//e^(-1/2*k^2)/(sum_k=-infty^infty e^(-1/2*k^2)): CDT
static const uint16_t standard_gauss_dist[] = { //5 elements for all, and each element in 16 bit (actually 10bit)
	39391u, 7675u, 599u, 18u//uint_16
}; //rcdt


// // e^(-1/2*k^2)
static const uint8_t standard_gauss_rcocdt[] = { //5 elements for all, and each element in 16 bit (actually 10bit)
	154u, 50u, 20u, 8u, 3u, 1u
    // 25786u, 41427u, 44916u, 45203u, 45212u
}; //rcocdt


//Return k with probability e^(-1/2*k^2)/(sum_k=0^infty e^(-1/2*k^2)) in unint16_t
int k_sampler(prng *p){
	uint16_t u , w;
	int k = 0;
	u = prng_get_u16(p);
	w = expon_dist_16bit[k];
	// printf("u = %d, w = %d \n", u, w);
	while((u - w) >> 16){
		w = expon_dist_16bit[++k];
	}
	return k;
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



// int k_sampler(prng *p){
// 	uint16_t u , w;
// 	int k = 0;
// 	u = prng_get_u16(p);
// 	w = standard_gauss_dist[k];
// 	printf("u = %d, w = %d \n", u, w);
// 	while((u - w) >> 16){
// 		w = standard_gauss_dist[++k];
// 	}
// 	return k;
// }


// int k_sampler(prng *p){
// 	uint8_t u , w;
// 	int k = 0;
// 	u = prng_get_u8(p);
// 	w = standard_gauss_rcocdt[k];
// 	while((u - w) >> 8){
// 		w = standard_gauss_rcocdt[++k];
// 		u = prng_get_u8(p);
// 	}
// 	return k;
// }





// bool AlgorithmH(prng *p) {
// 	float h_a, h_b;
// 	h_a = generate_uniform_x(p);

// 	// less than the half
// 	if (h_a > 0.5) return true;
// 	else {
// 		for (;;) {
// 			h_b = generate_uniform_x(p);
// 			if (h_b > h_a)
// 				return false;
// 			else
// 				h_a = generate_uniform_x(p);
// 		//	else  // numbers are equal - need higher precision
// 		//		return AlgorithmHDouble(p);
// 			if (h_a > h_b)
// 				return true;
// 		//	else if (h_a == h_b)  // numbers are equal - need higher precision
// 		//		return AlgorithmHDouble(p);
// 		}
// 	} 
// //	else { // numbers are equal - need higher precision
// //	return true;
// 		//	return AlgorithmHDouble(p);
// //	}
// }




bool AlgorithmP(prng *p, int n) {
	while (n-- && AlgorithmH(p)) {}
	return n < 0;
}


int AlgorithmG(prng *p) {
	int n = 0;
	while (AlgorithmH(p)) ++n;
	return n;
}
/*
bool AlgorithmBDouble(prng *p,int k, float x) {
	float y = x;
	int n = 0, m = 2 * k + 2;
	float z, r;

	for (;; ++n) {
		z = generate_uniform_x(p);
		if (!(z < y)) break;
		r = generate_uniform_x(p);
		if (!(r < (2 * k + x) / m)) break;
		y = z;
	}

	return (n % 2) == 0;
}
*/

bool AlgorithmB(prng *p, int k, float x) {
	float y = x;
	int n = 0, m = 2 * k + 2;
	float z, r;
	float rTemp;

	for (;; ++n) {
		z = generate_uniform_x(p);
		if (z > y) {
			break;
		} else {
			r = generate_uniform_x(p);
			rTemp = (2 * k + x) / m;
			if (r > rTemp)
				break;
			else
				y = z;
			//else  // r == Temp - need double precision
			//	return AlgorithmBDouble(p, k, x);
		} 
		//else {  // z == x - need double precision
		//	return AlgorithmBDouble(p, k, x);
		//}
	}

	return (n % 2) == 0;
}


//For center = 0 and sigma = 1024 is an integer
int Improved_Karney_for_Sampler2(prng *p) {
	int k, s;
	while(1) {
		// STEP 1 Sample k from N(0,1) Half Discrete Gauss Sampler
		k = ks_sampler(p,&s);
		// STEP 3 ȷ�Ϸ���

		// s = generate_uniform_sign(p); 
		// if(!s) s = -1;
		
		// STEP 4 ����С������ 
		// float di0 = stddev * k + s * mean;
		// int i0 = ceil(di0);
		// float x0 = (i0 - di0) / stddev;
		int j =  prng_get_u16(p)>>6; //random generate a 10 bit number
		// if ((j&k&s) == 0) continue;
		if(j == 0 && s == 0 && k == 0) continue;
		float x = j / 1024.;
		// STEP 5  
		// int h = k + 1;
		// while (h-- && AlgorithmB(p,k, x)) {}
		// if (!(h < 0)) continue; 
		if(generate_uniform_x(p) > f_exp(-1./2 * x *(2.*k+ x))) continue;
		//STEP 6 
		return (2*s-1) * ((k<<10) | j); 
	}
	return 0;
}



int DiscreteGaussian_Karney(prng *p, float mean, float stddev) {
	int k, s;

	while(1) {
		//STEP 1 ������������ 
		k = AlgorithmG(p);
		//STEP 2 ͨ���ܾ�������������Ը��� 
		if (!AlgorithmP(p, k * (k - 1))) continue;
		// STEP 3 ȷ�Ϸ���
		// k = ks_sampler(p,&s);

		s = generate_uniform_sign(p); 
		if(!s) s = -1;
		
		// STEP 4 ����С������ 
		float di0 = stddev * k + s * mean;
		int i0 = (int)(di0+1);
		float x0 = (i0 - di0) / stddev;
		int j = generate_uniform_random(p, (int)(stddev+1)); // С������ 
		float x = x0 + j / stddev;
		if (!(x < 1) || (x == 0 && s < 0 && k == 0)) continue;
		// STEP 5 ͨ���ܾ�����С������Ը��� 
		// int h = k + 1;
		// while (h-- && AlgorithmB(p,k, x)) {}
		// if (!(h < 0)) continue; 
		if(generate_uniform_x(p) > f_exp(-1./2 * x *(2.*k+ x))) continue;
		//STEP 6 �ϲ�������С�������ؽ�� 
		return s * (i0 + j); 
	}
	return 0;
}