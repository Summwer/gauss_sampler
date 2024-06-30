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

