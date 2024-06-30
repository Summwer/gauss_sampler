#include "sampler.h"

/*
 *
 * ******************************==
 ***Attention!!!************************************************
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


 int knuth_yao(float sigma, float center,prng *p){
    // center = -1.;
    // sigma = 1.7;
    
    int result; 
    static uint8_t first_time=1;
    //第一个问题：最初的初始化的时候
    if(first_time){
        root=(node *)malloc(sizeof (node));
        binary_p_mat=(uint32_t *) malloc((Pmat_row_size+1)*sizeof (uint32_t));
        expansion_margin=(node **)malloc(max_margin_size*sizeof (node *));
        changed = pre_knuthyao(center,sigma, root,  x_values,binary_p_mat,expansion_margin);
        current_sigma=sigma; current_center=center;
        first_time=0;
    }
    else{
        if(current_sigma!=sigma || current_center!= center){
        tree_destroyer(root);
        //还有一个部分
        free(binary_p_mat);
        free(expansion_margin);
        root=(node *)malloc(sizeof (node));
        binary_p_mat=(uint32_t *) malloc((Pmat_row_size+1)*sizeof (uint32_t));
        expansion_margin=(node **)malloc(max_margin_size*sizeof (node *));
        changed = pre_knuthyao(center,sigma, root,  x_values,binary_p_mat,expansion_margin);
        current_sigma=sigma; current_center=center;
        }
    }

    // return DiscreteGaussian_Karney(&spc->p,center,sigma); 

    float average=0;
    float var=0;
    // int acctual_sample_num=0;
    if(changed) {
        for (int i = 0;; i++) {
            uint64_t random_bits1=prng_get_u64(p);
            printf("%lu\n", random_bits1);
            result = knuth_yao_sampling(random_bits1, root);
            if (result == Pmat_row_size + 1) {
                //discard
                continue;
            }
            else
                break;
        }
    }
    else{
        uint64_t random_bits1=prng_get_u64(p);
       
        result = knuth_yao_sampling(random_bits1, root);
    }
    return x_values[result];
}

// Fixed sigma = 0.75 and center = 0
int sampler_1(void *ctx){
    double sigma = 0.75;
    double center = 0;
    //int z = DiscreteGaussian_Karney(&ctx,mu,sigma);

    sampler_context *spc;
    spc=ctx;

    return knuth_yao(sigma, center,&spc->p);
}


// Fixed sigma = 1024 and center = 0
int sampler_2(void *ctx){
    double sigma = 1024;
    double center = 0;

    // int z = 0;
    sampler_context *spc;
    spc=ctx;

    return knuth_yao(sigma, center,&spc->p);
}

// Fixed sigma = 1.5 and center c is uniformly distributed over [0,1)
int sampler_3(void *ctx, double center){
    double sigma = 1.5;

    // int z = 0;


    // return z;
    sampler_context *spc;
    spc=ctx;
    
    
    return knuth_yao(sigma, center,&spc->p);
}

// sigma is uniformly distributed over (0.8,1.6) and center is uniformly distributed over [0,1)
int sampler_4(void *ctx, double sigma, double center){
    // int z = 0;
    // return z;
    sampler_context *spc;
    spc=ctx;
    
    return knuth_yao(sigma, center,&spc->p);
}