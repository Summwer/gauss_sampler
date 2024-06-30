#include <stdint.h>
#include "stdio.h"
#include "time.h"
#include <stdlib.h>
#include <stdbool.h>
#include "math.h"
#include "sampler.h"


#define Pmat_row_size 17

unsigned long denominator=0x800000;
const float ln2=0.69314718056;
const float pi=3.1415926535;
const float inv_sqrt_2pi=0.398942291737;
const float exp_table[]={1.000000, 2.718282, 7.389056 ,20.085537 ,54.598148, 148.413162 ,403.428802, 1096.633179, 2980.958008, 8103.083984,
1.000000, 1.105171, 1.221403 ,1.349859, 1.491825 ,1.648721, 1.822119, 2.013753, 2.225541 ,2.459603,
1.000000, 1.010050, 1.020201, 1.030455, 1.040811 ,1.051271, 1.061837 ,1.072508 ,1.083287, 1.094174
};
const float exp_10=22026.465795;





float fast_log2(float x)
{
    int trunc_step=5;
    long i;
    float y=x;
    i=*(long *)&y; //把y的bit形式轉換成整數，以方便shift,float不能執行shift
    long Mantissa=i & 0x007FFFFF;
    long Exponent=(i>>23) & 0x000000FF;
    float term=1-2/(2+( (float) Mantissa )/(float) denominator);
    float acc=0;

    float term_square=term*term;
    float term_quadruple=term_square*term_square;
    float term_8_th=term_quadruple*term_quadruple;

    acc+=term;
    acc+=term_square*term/3.0;
    acc+=term_quadruple*term/5.0;
    //acc+=term*term_quadruple*term_square/7.0;
    //acc+=term*term_8_th/9.0;
    //好像其实算到3.0已经没多少差距了
    acc*=2;
    acc/=ln2;
    acc+=Exponent-127;
    return acc;
}

float exponent(float x) //x必须是负数, x<20.
{
    if(x>0) {
        printf("wrong,x should be below 0\n");
        return 1;
    }
    float abs_x=-x;
    if(abs_x>=20){
        printf("x too small\n");
        return 0;
    }

    float tmp=abs_x*100;
    float residue=tmp-(int)tmp;
    residue=-residue/100.0;


    int i_tmp=(int)tmp;
    int third_digit=i_tmp%10;
    i_tmp-=third_digit;
    i_tmp/=10;
    int second_digit=i_tmp%10;
    i_tmp-=second_digit;
    i_tmp/=10;
    int first_digit=i_tmp%10;
    i_tmp-=first_digit;
    i_tmp/=10;
    int zero_digit=i_tmp%10;
    float acc;
    acc=exp_table[first_digit]*exp_table[second_digit+10]*exp_table[third_digit+20];
    if(zero_digit!=0) acc*=exp_10;
    residue=1+residue+residue* residue/2.0;

    return residue/acc;
}



//coef是a0,a1,a2,a3,a4......这种顺序存下去的
float Honer_algorithm(float x,float *coef,int length){
    float v=coef[length-1]*x+coef[length-2];
    for(int i=length-3;i>=0;i--){
        v=v*x+coef[i];
    }
    return v;
}
float f_exp(float x){ //直接默认x是负数
    float iexp=0.367879450321197510;
    float abs_x=-x;
    int integer_part=(int)abs_x;
    float remaining_part=abs_x-integer_part;

    //制表
    float exp_table[5];
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
    float major=1;
    for(int i=0;i<5;i++){
        if(binary_index[i]==1)
            major*=exp_table[i];
    }

    //计算次要部分，由于次要部分x是小于1 的不妨计算x/2的级数展开，最后square回去
    float X=remaining_part/2.0;
    float root_minor=1;
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
    float coef[8]={1,1,1.0/2,1.0/6,1.0/24,1.0/120,1.0/720,1.0/5040};
    root_minor= Honer_algorithm(-X,coef,8);
    return root_minor*root_minor*major;
}



bool pre_knuthyao(float mu, float sigma, node* root, int x_values[], uint32_t *binary_p_mat, node ** expansion_margin){
    float isigma=1/sigma;
    printf("Knuth-Yao: Sample in N(%.3f, %.3f)\n", mu, sigma);
    
    
    float sum=0; //记录目前的概率和
    float P_mat[Pmat_row_size+1];
    for(int i=0;i<Pmat_row_size;i++)
    {
        //以mu为中心前后半分
        int x=(i-Pmat_row_size/2)+(int)mu;
        x_values[i]=x;
        P_mat[i]= expf(-(x-mu)*(x-mu)*isigma*isigma/2); //必须替换成自己写的exp
        //P_mat[i]=f_exp(-(x-mu)*(x-mu)*isigma*isigma/2); 
        sum+=P_mat[i];
    }

    
    
    int percision=16;
    bool changed= make_sum_to_one(P_mat,sum,binary_p_mat,Pmat_row_size,percision);
    //生成随机数
    // int sample_num=100000;
    //设置种子
    srand((unsigned int)10);
    
    node_constructor(root);
    //构建expansion margin
    
    expansion_margin[0]=root;

    //构建knuth yao tree
    int Pmat_row_actual_size=Pmat_row_size;
    if(changed) Pmat_row_actual_size++;
    tree_constructor(expansion_margin,1,binary_p_mat,Pmat_row_actual_size,percision,0x80000000);
    return changed;
}


// int main(int argc, char *argv[])
// {   
//     // float mu=-1, sigma = 1.7;
//     float mu=-1, sigma = 1.7;
//     int  output = atoi(argv[1]);
//     FILE *data;
//     //生成概率表P mat，然后依照这个表来进行

//     //从这里开始计时，让程序只跑一秒
//     clock_t start,finish;
//     start=clock();
    
//     bool changed = pre_knuthyao(mu, sigma);
    
//     if(output){
//         data= fopen("output.txt","w");
//         if(data==NULL){
//             printf("can't open the file\n");
//         }
//     }
    
//     float average=0;
//     float var=0;
//     int acctual_sample_num=0;
//     if(changed) {
//         for (int i = 0;; i++) {
//             uint64_t random_bits = rand();
//             random_bits <<= 32;
//             random_bits |= rand();
//             int result = knuth_yao_sampling(random_bits, root);
//             if (result == Pmat_row_size + 1) {
//                 //discard
//                 continue;
//             }
//             acctual_sample_num++;
//             if(output)
//                 fprintf(data, "%d ", x_values[result]);
//             //如果现在时间超过1s就停下循环
//             if (clock() - start > CLOCKS_PER_SEC) {
//                 break;
//             }
//             //average+=x_values[result];
//             //var+=(x_values[result]+1.0)*(x_values[result]+1.0);
//         }
//     }
//     else{
//         for (int i = 0;; i++) {
//             uint64_t random_bits = rand();
//             random_bits <<= 32;
//             random_bits |= rand();
//             int result = knuth_yao_sampling(random_bits, root);
//             acctual_sample_num++;
//             if(output)
//                 fprintf(data, "%d ", x_values[result]);
//             //如果现在时间超过1s就停下循环
//             if (clock() - start > CLOCKS_PER_SEC) {
//                 break;
//             }
//             //average+=x_values[result];
//             //var+=(x_values[result]+1.0)*(x_values[result]+1.0);
//         }
//     }
//     //average/=(float)acctual_sample_num;
//     //var/=(float)acctual_sample_num;
//     //float sigma=sqrt(var);
    
//     //printf("average=%f,sigma=%f\n",average,sigma);
//     if(output)
//         fclose(data);
//     tree_destroyer(root);
//     free(expansion_margin);
//     printf("Generate %d samples in 1 seconds.\n",acctual_sample_num);
// }

/*
void make_table_for_exp(FILE *f,float step)
{
    for(int i=0;i<10;i++)
    {
        fprintf(f,"%f ",exp(i*step));
    }
    fprintf(f,"\n");
}
*/

float gaussian_function(float sigma,float x) //计算高斯函数值
{
    return inv_sqrt_2pi/sigma* exponent(-x*x/(2*sigma*sigma));
}
/*expansion margin 是一个node的list，里面装着的是目前应该开拓的node的集合
 * margin size 是目前margin的大小，p是Pmat的列表形式，其每一个元素都是一个概率，p_length是有多少个概率
 * percision是每个概率的精度（对应Pmat的行的长度），index是用来读取其中一个bit的
 * 必须注意的是epansion_margin的长度是固定的，所以要预先留够足够的空间来存储这些东西
 * */
void tree_constructor(struct node** expansion_margin,int margin_size,uint32_t * p,int p_length,int percision,uint32_t index)
{
    //先初始化子节点
    for(int i=0;i<margin_size;i++)
    {
        expansion_margin[i]->left=(node *) malloc(sizeof(node));
        expansion_margin[i]->right=(node *)malloc(sizeof(node));
        node_constructor(expansion_margin[i]->left);
        node_constructor(expansion_margin[i]->right);
    }
    int current_cursor=2*margin_size-1; //从右向左遍历,cursor是用来标记intermediate node与terminal node的分界点的
    bool left_or_right=false; //true为left 否则为right
    for(int i=p_length-1;i>=0;i--) //从下向上读，这与论文 High Precision Discrete Gaussian Sampling on FPGAs 一致
    {
        if(p[i]&index){//这个位置是1，设置node 的type并且cursor左移一个
            if(!left_or_right){
                expansion_margin[current_cursor/2]->right->type=i;
            }
            else{
                expansion_margin[current_cursor/2]->left->type=i;
            }
            current_cursor--;
            left_or_right=!left_or_right;
        }
    }

    int new_margin_size=current_cursor+1; //计算新的margin size,这是下一层的情况
    if(new_margin_size<=0) //没有下一层了
    {
        return;
    }
    //更改expansion margin
    left_or_right=(current_cursor%2)==0;
    for(int i=current_cursor;i>=0;i--) //注意顺序必须从大到小
    {
        if(left_or_right){
            expansion_margin[i]=expansion_margin[i/2]->left;
        }
        else{
            expansion_margin[i]=expansion_margin[i/2]->right;
        }
        left_or_right=!left_or_right;
    }
    uint32_t new_index=index>>1;
    tree_constructor(expansion_margin,new_margin_size,p,p_length,percision,new_index);
}

void tree_destroyer(node * root)
{
    if(!root->left)
    {
        //走到头了
        free(root);
        return;
    }
    tree_destroyer(root->left);
    tree_destroyer(root->right);
}

int knuth_yao_sampling(uint64_t random_bits, node * tree)
{
    node *current_tree=tree;
    //0.5概率走左右,0左，1右边
    uint64_t index=1;

    for(uint8_t i=0;i<64;i++)
    {   
        //判断走到头没有
        if(current_tree->type!=255){
            //走到头了
            return current_tree->type;
        }

        //接着走，取出bit并判断0还是1
        if((random_bits&index)&index)
        {
            //向右走
            current_tree=current_tree->right;
        }
        else{
            //向左走
            current_tree=current_tree->left;
        }
        //更新index
        index=index<<1;
    }
    // return 0;
    printf("\n");
}

//预计输入的是一个float的概率值但是我们需要的是knuthyao算法中的那种二进制形态
uint32_t float_to_binary(float x,int percision) //默认x小于1
{
    uint32_t acc=0x00000000;
    uint32_t index=0x80000000;
    for(int i=0;i<percision;i++)
    {
        x*=2;
        int flag=(int)x;
        x-=flag;
        /*
        if(flag){
            acc|=index;
        }
        */
        acc |= index*flag;
        index>>=1;
    }
    return acc;
}
//返回值表示是否改变了这个pmat,因为传进来的不一定加起来和为1
bool make_sum_to_one(float * p_real ,float sum,uint32_t * p,uint8_t current_num,int percision){
    //检查是否sum to one
    //我没有检查是不是加起来超过1了，如果是的话，应该处理（主要依赖的是precision的截断效果认为会比1小）
    
    uint32_t acc=0x00000000;
    for(uint8_t i =0;i<current_num;i++)
    {
        p_real[i]/=sum;  //这个可以考虑用AVX优化,尽量少除法
        uint32_t tmp= float_to_binary(p_real[i],percision); //调用函数很多次，能不能向量化，调用一次，这样速度会快吗？？
        p[i]=tmp;
        acc+=tmp;
    }
    if(acc){
        //说明加起来并不是1
        acc=~acc;
        acc+=1;
        p[current_num]=acc;
        return true;
    }
    return false;
}
void test_func_print_uint32(uint32_t p)
{
    uint32_t index=0x80000000;
    for(int i=0;i<32;i++){
        if(index&p){
            printf("1\n");
        }else{
            printf("0\n");
        }
        index>>=1;
    }
    printf("\n");
}
void node_constructor(node *r){
    r->left=r->right= NULL;
    r->type=255;
}
