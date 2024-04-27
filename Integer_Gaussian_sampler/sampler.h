#ifndef INTEGER_GAUSSIAN_SAMPLER__
#define INTEGER_GAUSSIAN_SAMPLER__


/**************************************************************************
 ************************     API     *************************************
 **************************************************************************
 */


int sampler_1(void *ctx);

int sampler_2(void *ctx);

int sampler_3(void *ctx, double center);

int sampler_4(void *ctx, double sigma, double center);


#endif