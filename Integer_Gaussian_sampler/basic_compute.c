#include "sampler.h"

//coef是a0,a1,a2,a3,a4......这种顺序存下去的
double Honer_algorithm(double x,double *coef,int length){
    double v=coef[length-1]*x+coef[length-2];
    for(int i=length-3;i>=0;i--){
        v=v*x+coef[i];
    }
    return v;
}

double f_exp(double x){ //直接默认x是负数
    double iexp=0.367879450321197510;
    double abs_x=-x;
    int integer_part=(int)abs_x;
    double remaining_part=abs_x-integer_part;

    //制表
    double exp_table[5];
    int exp_potent[5]={1,2,4,8,16};
    exp_table[0]=iexp;
    exp_table[1]=exp_table[0]*exp_table[0];
    exp_table[2]=exp_table[1]*exp_table[1];
    exp_table[3]=exp_table[2]*exp_table[2];
    exp_table[4]=exp_table[3]*exp_table[3];
    //依照快速幂算法计算整数部分,最多计算到e-32次方
    int binary_index[5];
    binary_index[4]=integer_part/16;
    if(binary_index[4]>=2) return 0; //溢出了，额外处理
    integer_part-=binary_index[4]*16;
    binary_index[3]=integer_part/8;
    integer_part-=binary_index[3]*8;
    binary_index[2]=integer_part/4;
    integer_part-=binary_index[2]*4;
    binary_index[1]=integer_part/2;
    integer_part-=binary_index[1]*2;
    binary_index[0]=integer_part;
    //计算主要部分
    double major=1;
    for(int i=0;i<5;i++){
        if(binary_index[i]==1)
            major*=exp_table[i];
    }

    //计算次要部分，由于次要部分x是小于1 的不妨计算x/2的级数展开，最后square回去
    double X=remaining_part/2.0;
    double root_minor=1;
    /*
    root_minor+=(-X);
    root_minor+=X*X/2;
    root_minor+=-X*X*X/6;
    root_minor+=X*X*X*X/24;
    root_minor+=-X*X*X*X*X/120;
    root_minor+=X*X*X*X*X*X/720;
    root_minor+=X*X*X*X*X*X*X/5040;
    */
    X=remaining_part/2.0;
    double coef[8]={1,1,1.0/2,1.0/6,1.0/24,1.0/120,1.0/720,1.0/5040};
    root_minor= Honer_algorithm(-X,coef,8);
    return root_minor*root_minor*major;
}

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

__m256 exp256_ps(__m256 x) {
/* Modified code. The original code is here: https://github.com/reyoung/avx_mathfun
*/
__m256   exp_hi        = _mm256_set1_ps(88.3762626647949f);
__m256   exp_lo        = _mm256_set1_ps(-88.3762626647949f);

__m256   cephes_LOG2EF = _mm256_set1_ps(1.44269504088896341);
__m256   cephes_exp_C1 = _mm256_set1_ps(0.693359375);
__m256   cephes_exp_C2 = _mm256_set1_ps(-2.12194440e-4);

__m256   cephes_exp_p0 = _mm256_set1_ps(1.9875691500E-4);
__m256   cephes_exp_p1 = _mm256_set1_ps(1.3981999507E-3);
__m256   cephes_exp_p2 = _mm256_set1_ps(8.3334519073E-3);
__m256   cephes_exp_p3 = _mm256_set1_ps(4.1665795894E-2);
__m256   cephes_exp_p4 = _mm256_set1_ps(1.6666665459E-1);
__m256   cephes_exp_p5 = _mm256_set1_ps(5.0000001201E-1);
__m256   tmp           = _mm256_setzero_ps(), fx;
__m256i  imm0;
__m256   one           = _mm256_set1_ps(1.0f);

        x     = _mm256_min_ps(x, exp_hi);
        x     = _mm256_max_ps(x, exp_lo);

  /* express exp(x) as exp(g + n*log(2)) */
        fx    = _mm256_mul_ps(x, cephes_LOG2EF);
        fx    = _mm256_add_ps(fx, _mm256_set1_ps(0.5f));
        tmp   = _mm256_floor_ps(fx);
__m256  mask  = _mm256_cmp_ps(tmp, fx, _CMP_GT_OS);    
        mask  = _mm256_and_ps(mask, one);
        fx    = _mm256_sub_ps(tmp, mask);
        tmp   = _mm256_mul_ps(fx, cephes_exp_C1);
__m256  z     = _mm256_mul_ps(fx, cephes_exp_C2);
        x     = _mm256_sub_ps(x, tmp);
        x     = _mm256_sub_ps(x, z);
        z     = _mm256_mul_ps(x,x);

__m256  y     = cephes_exp_p0;
        y     = _mm256_mul_ps(y, x);
        y     = _mm256_add_ps(y, cephes_exp_p1);
        y     = _mm256_mul_ps(y, x);
        y     = _mm256_add_ps(y, cephes_exp_p2);
        y     = _mm256_mul_ps(y, x);
        y     = _mm256_add_ps(y, cephes_exp_p3);
        y     = _mm256_mul_ps(y, x);
        y     = _mm256_add_ps(y, cephes_exp_p4);
        y     = _mm256_mul_ps(y, x);
        y     = _mm256_add_ps(y, cephes_exp_p5);
        y     = _mm256_mul_ps(y, z);
        y     = _mm256_add_ps(y, x);
        y     = _mm256_add_ps(y, one);

  /* build 2^n */
        imm0  = _mm256_cvttps_epi32(fx);
        imm0  = _mm256_add_epi32(imm0, _mm256_set1_epi32(0x7f));
        imm0  = _mm256_slli_epi32(imm0, 23);
__m256  pow2n = _mm256_castsi256_ps(imm0);
        y     = _mm256_mul_ps(y, pow2n);
        return y;
}