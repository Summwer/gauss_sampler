#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include "speed_print.h"
#include "cpucycles.h"


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

int DiscreteGaussian_Karney(double mean, double stddev) {
	int k, s;
	int stdd = stddev;
	if(stdd != stddev) ++stdd;  //stdd = ceil(stddev)
	double g;
	while(1) {
		//STEP 1 采样整数部分
		k = AlgorithmG();
		
		//STEP 2 通过拒绝调整整数的相对概率
		if (!AlgorithmP(k * (k - 1))) continue; //以exp(-1/2*k*(k-1))的概率接受整数k

		// printf("k = %d\n", k);
		// STEP 3 确认符号:s (+/-)
	        g = generate_uniform_random(1);
		if(g < 0.5) s = 1;
		else s = -1;
		// STEP 4 采样小数部分
		double di0 = stddev * k + s * mean;
		int i0 = ceil(di0);
		// printf("i0 = %lld \n", i0);
		// printf("di0= %f \n\n", stddev * k + s * mean);
		double x0 = ((double) i0 - di0) / stddev;
		// printf("x0 = %f\n", x0);
		// int j = generate_uniform_random(ceil(stddev) - 1); // 小数部分
		int j = generate_uniform_random(ceil(stdd)); // 小数部分
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
int main(int argc, char *argv[]) {
	// double mu = -1., sigma = 1.7;
	double mu = 0, sigma = 1024;
	printf("Karney: Sample in N(%.3f, %.3f)\n", mu, sigma);
	int i = 0, j = 0, output = atoi(argv[3]);
	FILE *file;
	if(output)
		file = fopen("output.txt", "w");  //输出到文件中 

	// clock_t start, finish;
	// start = clock();

	srand(time(NULL));
	
	clock_t start, finish;
	int NTESTS =0 ;
	int random_number;
	start = clock();
    // fprintf(file,"[");
	do{
		// t[i] = cpucycles();
		random_number = DiscreteGaussian_Karney(mu, sigma); 
		NTESTS +=1;
		if(output)
			printf("%d ", random_number);
		finish = clock();
	}while((finish - start)/CLOCKS_PER_SEC < 1.);
	// fprintf(file,"]");
	printf("Generate %d samples in 1 seconds.\n",NTESTS);
	if(output)
        fclose(file);

	return 0;
}

