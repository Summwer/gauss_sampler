#ifndef INTEGER_GAUSSIAN_SAMPLER__
#define INTEGER_GAUSSIAN_SAMPLER__

#include "../example/sampler/sampler.h"
#include <stdio.h>
#include <stdbool.h>
// #include "knuth_yao.h"

// #include "my_Karney_prng.h"

/**************************************************************************
 ************************     API     *************************************
 **************************************************************************
 */




int sampler_1(void *ctx);

int sampler_2(void *ctx);

int sampler_3(void *ctx, double center);

int sampler_4(void *ctx, double sigma, double center);


#endif

double f_exp(double x); //compute e^x

float generate_uniform_x(prng *p);
// float generate_uniform_random(prng *p, float x);
// int DiscreteGaussian_Karney(prng *p, float mean, float stddev); 
int Improved_Karney_for_Sampler2(prng *p); 


typedef struct node node;
struct node{
    int type;//设置255为intermediate节点，否则是terminal，并且值是所要的
    struct node * left;
    struct node * right;
};


//For Knuth-Yao
#define Pmat_row_size 17
#define max_margin_size Pmat_row_size+1

float current_sigma;//=-928.1; //为了能初始化成功
float current_center;//=-82.34;
node * root;//=NULL;
uint32_t *binary_p_mat;//=NULL;
node ** expansion_margin;//=NULL;
bool changed;
int x_values[Pmat_row_size+1];
bool pre_knuthyao(float mu, float sigma, node* root, int x_values[], uint32_t *binary_p_mat, node ** expansion_margin);
int knuth_yao_sampling(uint64_t random_bits,struct node * tree);
void node_constructor(struct node *);
void test_func_print_uint32(uint32_t);
uint32_t float_to_binary(float x,int percision);
void make_table_for_exp(FILE *,float);
float gaussian_function(float,float );
void tree_constructor(struct node**,int,uint32_t * ,int ,int ,uint32_t);
void tree_destroyer(struct node * root);
bool make_sum_to_one(float * p_real ,float sum,uint32_t * p,uint8_t current_num,int);




float generate_uniform_random(prng *p, float x);
int ks_sampler(prng *p,int *s);
int Improved_Karney_for_Sampler2(prng *p);
double f_exp(double x);


/*
 * Get an 16-bit random value from a PRNG.
 */
static inline unsigned
prng_get_u16(prng *p)
{
	unsigned v;

	v = p->ptr; //After prng_init, p->ptr = 0.
	//There is no enough value in prng p to generate a 16-bit random value.

	if (v >= (sizeof p->buf.d) - 3) { //3 = 2+1, 16 bits = 2 bytes
		Zf(prng_refill)(p);
		v = 0;
	}
	p->ptr = v + 2;

	return (uint16_t)p->buf.d[v + 0]
		| ((uint16_t)p->buf.d[v + 1] << 8);
}



