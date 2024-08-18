#include <stdio.h>
#include <immintrin.h>
/*     gcc -O3 -m64 -Wall -mavx2 -march=broadwell  expc.c    */

__m256d exp256_pd(__m256d x) {
/* Modified code. The original code is here: httpd://github.com/reyoung/avx_mathfun

*/

__m256d   exp_hi        = _mm256_set1_pd(88.3762626647949f);
__m256d   exp_lo        = _mm256_set1_pd(-88.3762626647949f);

__m256d   cephes_LOG2EF = _mm256_set1_pd(1.44269504088896341);
__m256d   cephes_exp_C1 = _mm256_set1_pd(0.693359375);
__m256d   cephes_exp_C2 = _mm256_set1_pd(-2.12194440e-4);

__m256d   cephes_exp_p0 = _mm256_set1_pd(1.9875691500E-4);
__m256d   cephes_exp_p1 = _mm256_set1_pd(1.3981999507E-3);
__m256d   cephes_exp_p2 = _mm256_set1_pd(8.3334519073E-3);
__m256d   cephes_exp_p3 = _mm256_set1_pd(4.1665795894E-2);
__m256d   cephes_exp_p4 = _mm256_set1_pd(1.6666665459E-1);
__m256d   cephes_exp_p5 = _mm256_set1_pd(5.0000001201E-1);
__m256d   tmp           = _mm256_setzero_pd(), fx;
__m256i  imm0;
__m256d   one           = _mm256_set1_pd(1.0f);

        x     = _mm256_min_pd(x, exp_hi);
        x     = _mm256_max_pd(x, exp_lo);

  /* express exp(x) as exp(g + n*log(2)) */
        fx    = _mm256_mul_pd(x, cephes_LOG2EF);
        

        fx    = _mm256_add_pd(fx, _mm256_set1_pd(0.5f));
        tmp   = _mm256_floor_pd(fx); //这个指令执行不了与avx512有兼容问题
        
__m256d  mask  = _mm256_cmp_pd(tmp, fx, _CMP_GT_OS);    
        mask  = _mm256_and_pd(mask, one);
        fx    = _mm256_sub_pd(tmp, mask);
        tmp   = _mm256_mul_pd(fx, cephes_exp_C1);
__m256d  z     = _mm256_mul_pd(fx, cephes_exp_C2);
        x     = _mm256_sub_pd(x, tmp);
        x     = _mm256_sub_pd(x, z);
        z     = _mm256_mul_pd(x,x);

__m256d  y     = cephes_exp_p0;
        y     = _mm256_mul_pd(y, x);
        y     = _mm256_add_pd(y, cephes_exp_p1);
        y     = _mm256_mul_pd(y, x);
        y     = _mm256_add_pd(y, cephes_exp_p2);
        y     = _mm256_mul_pd(y, x);
        y     = _mm256_add_pd(y, cephes_exp_p3);
        y     = _mm256_mul_pd(y, x);
        y     = _mm256_add_pd(y, cephes_exp_p4);
        y     = _mm256_mul_pd(y, x);
        y     = _mm256_add_pd(y, cephes_exp_p5);
        y     = _mm256_mul_pd(y, z);
        y     = _mm256_add_pd(y, x);
        y     = _mm256_add_pd(y, one);

  /* build 2^n */
        //imm0  = _mm256_cvttpd_epi64(fx);
        //手动转换
        double xv[4] __attribute__((aligned(32)));
        long long int yv[4] __attribute__((aligned(32)));
        
        _mm256_store_pd(xv,fx);
        for(int i=0;i<4;i++){
                yv[i]=xv[i]; 
        }
        imm0=_mm256_load_si256((__m256i*) yv);
        imm0  = _mm256_add_epi64(imm0, _mm256_set1_epi64x(0x3ff));
        imm0  = _mm256_slli_epi64(imm0, 52);
__m256d  pow2n = _mm256_castsi256_pd(imm0); //单纯解释成浮点数，按bit解释
        y     = _mm256_mul_pd(y, pow2n);
        return y;
}

int main(){
    int i;
    double xv[4];
    double yv[4];
    __m256d x = _mm256_setr_pd(1.1, 2.0, 3.0 ,4.0);
    __m256d y = exp256_pd(x);
    y=_mm256_fmaddsub_pd(x,x,y);
    _mm256_store_pd(xv,x);
    _mm256_store_pd(yv,y);

    for (i=0;i<4;i++){
        printf("i = %i, x = %f, y = %f \n",i,xv[i],yv[i]);
    }
    return 0;
}