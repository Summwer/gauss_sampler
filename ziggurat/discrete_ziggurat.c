/*
 * A simple implementation of the algorithm in this paper: Discrete Ziggurat: A Time-Memory
Trade-Off for Sampling from a Gaussian
Distribution over the Integers.

It is worthwhile to note that the pseudo codes are already contained in that paper and we didn't choose this 
algorithm in the final phase of the competition.
 */
#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include "math.h"
#include "stdbool.h"
#include "stdlib.h"
float gaussian_0(float ,float );
float inv_gaussian_0(float ,float );
bool partition(int m,float* y,float* x);
float sLine(int,int,int,float ,float ,float );
int sampler(float *,float *);
float sigma=1.7;
int t=12;
int m=7;
float S_base;
float S;
float pi=3.1415926;

int main(void) {
    srand((unsigned)time(NULL));
    S=S_base=sigma/(m* sqrtf(pi/2));
    float * x_array=(float *)malloc((m+1)*sizeof(float));
    float * y_array=(float *) malloc((m+1)*sizeof (float ));
    int c=1;
    while(partition(m,y_array,x_array)==false){
        c++;
        S=S_base*c;
    }

    int num=30;//the number of samples
    int *s=(int *) malloc(num*sizeof(int));
    for(int i=0;i<num;i++){
        s[i]= sampler(x_array,y_array);
        printf("%d\n",s[i]);
    }
    printf("fail time:%d",c);
    free(x_array);
    free(y_array);
}

float gaussian_0(float isigma,float x){
    return expf(-x*x*isigma*isigma*0.5);
}
float inv_gaussian_0(float sigma,float y){//y<1
    return sigma* sqrtf(-2*logf(y));
}


bool partition(int m,float *y,float *x){
    y[m]=0;x[0]=0;x[m]=t*sigma;
    for(int i=m-1;i>=0;i--){
        y[i]=y[i+1]+S/(1+(int)x[i+1]);
        x[i]= inv_gaussian_0(sigma,y[i]);
    }
    x[0]=0;
    if(y[0]>=1.0){
        return true;
    }
    return false;
}
float sLine(int i,int xi_1,int xi,float yi_1 ,float yi,float x){
    if(xi_1==xi) return -1;
    if(i==1){
        return (yi-1)*(x-xi)/(xi-xi_1);
    }
    else{
        return (yi-yi_1)*(x-xi)/(xi-xi_1);
    }
}
int sampler(float *x_array,float *y_array)
{
    while(true){
        int i=rand()%m+1;
        int s=((rand()%2)<<1)-1;
        int x=rand()%( 1+(int)(x_array[i]) );

        if (0<x&&x<=(int)(x_array[i-1])){
            return s*x;
        }
        else{
            if(x==0){
                int b=rand()%2;
                if(b) continue;
                return 0;
            }
            else{
                //sample a uniform random floating-number between y[i] and y[i-1]
                float y=(float)(rand()%60001)/60000.0*(y_array[i-1]-y_array[i]);
                if((int)x_array[i]+1<=sigma){
                    if(y<=sLine(i,(int)x_array[i-1],(int)x_array[i],y_array[i-1],y_array[i],x)||y<=(gaussian_0(1/sigma,x)-y_array[i])){
                        return s*x;
                    }
                    continue;
                }
                else{
                    if ((int)x_array[i-1]>=sigma){
                        if(y>=sLine(i,(int)x_array[i-1],(int)x_array[i],y_array[i-1],y_array[i],x-1)||y>(gaussian_0(1/sigma,x)-y_array[i])) continue;
                        return s*x;
                    }
                    else{
                        if(y<=(gaussian_0(1/sigma,x)-y_array[i])) return s*x;
                        else continue;
                    }
                }
            }
        }
    }
}
