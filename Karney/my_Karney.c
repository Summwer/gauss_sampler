#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include "speed_print.h"
#include "cpucycles.h"


#define ll long long
double generate_uniform_random(double x) { //[0, x)
	return ((double)rand() / RAND_MAX) * x;
}



bool AlgorithmHDouble() {
	double h_a, h_b;
	h_a = generate_uniform_random(1);
	// less than the half
	if (!(h_a < 0.5)) return true;
	for (;;) {
		h_b = generate_uniform_random(1);
		if (!(h_b < h_a))
			return false;
		else
			h_a = generate_uniform_random(1);
		if (!(h_a < h_b)) return true;
	}
}

bool AlgorithmH() {
	float h_a, h_b;
	h_a = generate_uniform_random(1);

	// less than the half
	if (h_a > 0.5) return true;
	if (h_a < 0.5) {
		for (;;) {
			h_b = generate_uniform_random(1);
			if (h_b > h_a)
				return false;
			else if (h_b < h_a)
				h_a = generate_uniform_random(1);
			else  // numbers are equal - need higher precision
				return AlgorithmHDouble();
			if (h_a > h_b)
				return true;
			else if (h_a == h_b)  // numbers are equal - need higher precision
				return AlgorithmHDouble();
		}
	} else { // numbers are equal - need higher precision
		return AlgorithmHDouble();
	}
}

bool AlgorithmP(int n) {
	while (n-- && AlgorithmH()) {}
	return n < 0;
}

int AlgorithmG() {
	int n = 0;
	while (AlgorithmH()) ++n;
	return n;
}

bool AlgorithmBDouble(int k, double x) {
	double y = x;
	int n = 0, m = 2 * k + 2;
	double z, r;

	for (;; ++n) {
		z = generate_uniform_random(1);
		if (!(z < y)) break;
		r = generate_uniform_random(1);
		if (!(r < (2 * k + x) / m)) break;
		y = z;
	}

	return (n % 2) == 0;
}


bool AlgorithmB(int k, double x) {
	float y = x;
	int n = 0, m = 2 * k + 2;
	float z, r;
	float rTemp;

	for (;; ++n) {
		z = generate_uniform_random(1);
		if (z > y) {
			break;
		} else if (z < y) {
			r = generate_uniform_random(1);
			rTemp = (2 * k + x) / m;
			if (r > rTemp)
				break;
			else if (r < rTemp)
				y = z;
			else  // r == Temp - need double precision
				return AlgorithmBDouble(k, x);
		} else {  // z == x - need double precision
			return AlgorithmBDouble(k, x);
		}
	}

	return (n % 2) == 0;
}
int cnt1 = 0, cnt2 = 0;
ll DiscreteGaussian_Karney(double mean, double stddev) {
	int k, s;
	
	double g;
	while(1) {
		//STEP 1 采样整数部分
		k = AlgorithmG();
		
		//STEP 2 通过拒绝调整整数的相对概率
		if (!AlgorithmP(k * (k - 1))) continue; //以exp(-1/2*k*(k-1))的概率接受整数k

		// printf("k = %d\n", k);
		// STEP 3 确认符号:s (+/-)
		while(1){
			g = generate_uniform_random(1); 
			if(g < 0.5) s = 1, ++cnt1;
			else if(g > 0.5) s = -1, ++cnt2; 
			else {
				continue;
			}
			break;
		}
		
		// STEP 4 采样小数部分
		double di0 = stddev * k + s * mean;
		ll i0 = ceil(di0);
		// printf("i0 = %lld \n", i0);
		// printf("di0= %f \n\n", stddev * k + s * mean);
		double x0 = ((double) i0 - di0) / stddev;
		// printf("x0 = %f\n", x0);
		// ll j = generate_uniform_random(ceil(stddev) - 1); // 小数部分
		ll j = generate_uniform_random(ceil(stddev)); // 小数部分
		// printf("%f\n", j);
		double x = x0 + (double) j / stddev;
		if (!(x < 1) || (x == 0 && s < 0 && k == 0)) continue;
		// printf("sample: %lld\n", s * (i0 + j));
		// STEP 5 通过拒绝调整小数的相对概率
		int h = k + 1;
		while (h-- && AlgorithmB(k, x)) {}
		if (!(h < 0)) continue; 
		//STEP 6 合并整数和小数并返回结果
		return s * (i0 + j); 
	}
	return 0;
}
int main() {
	srand(time(NULL));
	int i = 0, j = 0;
	FILE *file = fopen("output.txt", "w");  //������ļ��� 
//	clock_t start, stop; 
//	double ave = 0;
//	for(; j < 1; j++){
//		start = clock();

	// int NTESTS = 10000;
	// uint64_t t[NTESTS];

	clock_t start, finish;
	int NTESTS =0 ;
	ll random_number;
	start = clock();
    // fprintf(file,"[");
	do{
		// t[i] = cpucycles();
		random_number = DiscreteGaussian_Karney(-1.,1.7); 
		NTESTS +=1;
		printf("%lld,", random_number);
		finish = clock();
	}while((finish - start)/CLOCKS_PER_SEC < 1.);
	// fprintf(file,"]");
	printf("Generate %d samples in 1 seconds.",NTESTS);
	// print_results("Cost of of Sampler: ", t, NTESTS);

//		stop = clock();	
//		double duration=(double)(stop-start)/CLOCKS_PER_SEC * 1000; 
//	    printf("%lf\n",duration);
//	    ave += duration;
//	}
//	printf("%lf\n",ave/20);
	return 0;
}

