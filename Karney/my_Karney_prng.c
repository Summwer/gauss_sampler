#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include "speed_print.h"
#include "cpucycles.h"
#include "sampler.h"

#define ll long long
// double generate_uniform_random(prng *p, double x) { //生成随机数[0, x]
// 	uint64_t lo = prng_get_u64(p); //Generate a 64-bit random value

// 	return ((double) lo / (double) 0xffffffffffffffff) * x;;
// }

double generate_uniform_random(prng *p, double x) { //生成随机数[0, x]
	return ((double)rand() / RAND_MAX) * x;
}



bool AlgorithmHDouble(prng *p) {
	double h_a, h_b;
	h_a = generate_uniform_random(p,1);
	// less than the half
	if (!(h_a < 0.5)) return true; 
	for (;;) {
		h_b = generate_uniform_random(p,1);
		if (!(h_b < h_a))
			return false;
		else
			h_a = generate_uniform_random(p,1);
		if (!(h_a < h_b)) return true;
	}
}

bool AlgorithmH(prng *p) {
	float h_a, h_b;
	h_a = generate_uniform_random(p,1);

	// less than the half
	if (h_a > 0.5) return true;
	if (h_a < 0.5) {
		for (;;) {
			h_b = generate_uniform_random(p,1);
			if (h_b > h_a)
				return false;
			else if (h_b < h_a)
				h_a = generate_uniform_random(p,1);
			else  // numbers are equal - need higher precision
				return AlgorithmHDouble(p);
			if (h_a > h_b)
				return true;
			else if (h_a == h_b)  // numbers are equal - need higher precision
				return AlgorithmHDouble(p);
		}
	} else { // numbers are equal - need higher precision
		return AlgorithmHDouble(p);
	}
}

bool AlgorithmP(prng *p, int n) {
	while (n-- && AlgorithmH(p)) {}
	return n < 0;
}

int AlgorithmG(prng *p) {
	int n = 0;
	while (AlgorithmH(p)) ++n;
	return n;
}

bool AlgorithmBDouble(prng *p,int k, double x) {
	double y = x;
	int n = 0, m = 2 * k + 2;
	double z, r;

	for (;; ++n) {
		z = generate_uniform_random(p, 1);
		if (!(z < y)) break;
		r = generate_uniform_random(p, 1);
		if (!(r < (2 * k + x) / m)) break;
		y = z;
	}

	return (n % 2) == 0;
}


bool AlgorithmB(prng *p, int k, double x) {
	float y = x;
	int n = 0, m = 2 * k + 2;
	float z, r;
	float rTemp;

	for (;; ++n) {
		z = generate_uniform_random(p, 1);
		if (z > y) {
			break;
		} else if (z < y) {
			r = generate_uniform_random(p, 1);
			rTemp = (2 * k + x) / m;
			if (r > rTemp)
				break;
			else if (r < rTemp)
				y = z;
			else  // r == Temp - need double precision
				return AlgorithmBDouble(p, k, x);
		} else {  // z == x - need double precision
			return AlgorithmBDouble(p, k, x);
		}
	}

	return (n % 2) == 0;
}
int cnt1 = 0, cnt2 = 0;
ll DiscreteGaussian_Karney(void *ctx, double mean, double stddev) {
	int k, s;

	sampler_context *spc;
	spc = ctx;
	
	double g;
	while(1) {
		//STEP 1 采样整数部分 
		k = AlgorithmG(&spc->p);
		//STEP 2 通过拒绝调整整数的相对概率 
		if (!AlgorithmP(&spc->p, k * (k - 1))) continue;
		// STEP 3 确认符号
		while(1){
			g = generate_uniform_random(&spc->p, 1); 
			if(g < 0.5) s = 1, ++cnt1;
			else if(g > 0.5) s = -1, ++cnt2; 
			else {
				printf("hahahaha");
				continue;
			}
			break;
		}
		
		// STEP 4 采样小数部分 
		double di0 = stddev * k + s * mean;
		ll i0 = ceil(di0);
		double x0 = (i0 - di0) / stddev;
		ll j = generate_uniform_random(&spc->p, ceil(stddev) - 1); // 小数部分 
		double x = x0 + j / stddev;
		if (!(x < 1) || (x == 0 && s < 0 && k == 0)) continue;

		// STEP 5 通过拒绝调整小数的相对概率 
		int h = k + 1;
		while (h-- && AlgorithmB(&spc->p,k, x)) {}
		if (!(h < 0)) continue; 
		//STEP 6 合并整数和小数并返回结果 
		return s * (i0 + j); 
	}
	return 0;
}
int main() {
	srand(time(NULL));
	int i = 0, j = 0;
	FILE *file = fopen("output.txt", "w");  //输出到文件中 
//	clock_t start, stop; 
//	double ave = 0;
//	for(; j < 1; j++){
//		start = clock();

	sampler_shake256_context rng;
    sampler_context sc;
	//生成随机数
    sampler_shake256_init(&rng); 
    sampler_shake256_inject(&rng, (const void *)"test sampler", 12);
    sampler_shake256_flip(&rng);
    Zf(prng_init)(&sc.p, &rng); 

	clock_t start, finish;
	int NTESTS =0 ;
	ll random_number;
	start = clock();
    // fprintf(file,"[");
	do{
		// t[i] = cpucycles();
		random_number = DiscreteGaussian_Karney(&sc,0,1.7); 
		NTESTS +=1;
		finish = clock();
	}while((finish - start)/CLOCKS_PER_SEC < 1.);
	// fprintf(file,"]");
	printf("Generate %d samples in 1 seconds.",NTESTS);
//		stop = clock();	
//		double duration=(double)(stop-start)/CLOCKS_PER_SEC * 1000; 
//	    printf("%lf\n",duration);
//	    ave += duration;
//	}
//	printf("%lf\n",ave/20);
	return 0;
}

