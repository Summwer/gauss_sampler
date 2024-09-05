/*
We implemented the knuth-yao algoritm to generate discrete gaussians over integers.
The original algorithm is in this paper: High Precision Discrete Gaussian Sampling on FPGAs.
Although there is no pseudo code in the original paper, it contains a description of the algorithm.
As claimed in MW17, it is rather efficient. However, its space complexity is higher than table based methods.

The results will be presented in output.txt.
*/

#include <stdint.h>
#include "stdio.h"
#include "time.h"
#include <stdlib.h>
#include <stdbool.h>
#include "math.h"

typedef struct node node;
struct node{
    int type;//When it equals 255, it is an intermediate node. Otherwise, it is a terminal node.
    struct node * left;
    struct node * right;
};
void node_constructor(struct node *);
void test_func_print_uint32(uint32_t);
uint32_t float_to_binary(float x,int percision);
void make_table_for_exp(FILE *,float);
float gaussian_function(float,float );
void tree_constructor(struct node**,int,uint32_t * ,int ,int ,uint32_t);
void tree_destroyer(struct node * root);
int knuth_yao_sampling(uint64_t random_bits,struct node * tree);
bool make_sum_to_one(float * p_real ,float sum,uint32_t * p,uint8_t current_num,int);
unsigned long denominator=0x800000;
const float ln2=0.69314718056;
const float pi=3.1415926535;
const float inv_sqrt_2pi=0.398942291737;
const float exp_table[]={1.000000, 2.718282, 7.389056 ,20.085537 ,54.598148, 148.413162 ,403.428802, 1096.633179, 2980.958008, 8103.083984,
1.000000, 1.105171, 1.221403 ,1.349859, 1.491825 ,1.648721, 1.822119, 2.013753, 2.225541 ,2.459603,
1.000000, 1.010050, 1.020201, 1.030455, 1.040811 ,1.051271, 1.061837 ,1.072508 ,1.083287, 1.094174
};//precomputed results by the exp() function

const float exp_10=22026.465795;


float fast_log2(float x)
{
    int trunc_step=5;
    long i;
    float y=x;
    i=*(long *)&y; 
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
    acc*=2;
    acc/=ln2;
    acc+=Exponent-127;
    return acc;
}

//a very simple method to compute exp(x). Firstly, we use the exp_table to compute the main part of exp(x),
//then we use Taylor's formula to compute the rest

float exponent(float x) //x<0 and -x<20.
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


int main(int argc, char *argv[])
{   
    // float mu=-1, sigma = 1.7;
    float mu=-1, sigma = 1.7;
    int  output = atoi(argv[1]);
    FILE *data;
    //generate P_mat, which is a matrix containing probabilities.

    //record time, we only allow the program to run for one sec.
    clock_t start,finish;
    start=clock();
    
    float isigma=1/sigma;
    printf("Knuth-Yao: Sample in N(%.3f, %.3f)\n", mu, sigma);
    int Pmat_row_size=17;
    int x_values[Pmat_row_size+1];
    float sum=0; //the sum of current "probabilities".  It will be used for normalization.
    float P_mat[Pmat_row_size+1];
    for(int i=0;i<Pmat_row_size;i++)
    {
        int x=(i-Pmat_row_size/2)+(int)mu;
        x_values[i]=x;
        P_mat[i]= exponent(-(x-mu)*(x-mu)*isigma*isigma/2);
        sum+=P_mat[i];
    }

    uint32_t *binary_p_mat;
    binary_p_mat=(uint32_t *) malloc((Pmat_row_size+1)*sizeof (uint32_t));
    int percision=16;
    bool changed= make_sum_to_one(P_mat,sum,binary_p_mat,Pmat_row_size,percision);
  
    srand((unsigned int)10);
    node * root=(node *)malloc(sizeof (node));
    node_constructor(root);
    //build expansion margin
    uint16_t max_margin_size=Pmat_row_size+1;
    node ** expansion_margin=(node **)malloc(max_margin_size*sizeof (node *));
    expansion_margin[0]=root;

    //construct DDG tree
    int Pmat_row_actual_size=Pmat_row_size;
    if(changed) Pmat_row_actual_size++;
    tree_constructor(expansion_margin,1,binary_p_mat,Pmat_row_actual_size,16,0x80000000);

    if(output){
        data= fopen("output.txt","w");
        if(data==NULL){
            printf("can't open the file\n");
        }
    }
    
    float average=0;
    float var=0;
    int acctual_sample_num=0;
    if(changed) {
        for (int i = 0;; i++) {
            uint64_t random_bits = rand();
            random_bits <<= 32;
            random_bits |= rand();
            int result = knuth_yao_sampling(random_bits, root);
            if (result == Pmat_row_size + 1) {
                //discard
                continue;
            }
            acctual_sample_num++;
            if(output)
                fprintf(data, "%d ", x_values[result]);
            
            if (clock() - start > CLOCKS_PER_SEC) {
                break;
            }
            
        }
    }
    else{
        for (int i = 0;; i++) {
            uint64_t random_bits = rand();
            random_bits <<= 32;
            random_bits |= rand();
            int result = knuth_yao_sampling(random_bits, root);
            acctual_sample_num++;
            if(output)
                fprintf(data, "%d ", x_values[result]);
            if (clock() - start > CLOCKS_PER_SEC) {
                break;
            }
           
        }
    }
    if(output)
        fclose(data);
    tree_destroyer(root);
    free(expansion_margin);
    printf("Generate %d samples in 1 seconds.\n",acctual_sample_num);
}


void make_table_for_exp(FILE *f,float step)
{
    for(int i=0;i<10;i++)
    {
        fprintf(f,"%f ",exp(i*step));
    }
    fprintf(f,"\n");
}

float gaussian_function(float sigma,float x) 
{
    return inv_sqrt_2pi/sigma* exponent(-x*x/(2*sigma*sigma));
}
/*expansion margin is list of nodes，which contains the DDG tree nodes that need to "give birth" to its children.
 * margin size is the current size，p is an array containing P_mat's elements, p_length is the length of this array.
 * percision is used to truncate the probabilities.（corresponds to the number of cols in Pmat），index is used to get a single bit.
 * */
void tree_constructor(struct node** expansion_margin,int margin_size,uint32_t * p,int p_length,int percision,uint32_t index)
{
    //initialization
    for(int i=0;i<margin_size;i++)
    {
        expansion_margin[i]->left=(node *) malloc(sizeof(node));
        expansion_margin[i]->right=(node *)malloc(sizeof(node));
        node_constructor(expansion_margin[i]->left);
        node_constructor(expansion_margin[i]->right);
    }
    int current_cursor=2*margin_size-1; //traverse from right to left ,cursor is used to mark the "boarder" between intermediate nodes and terminal nodes
    bool left_or_right=false; 
    for(int i=p_length-1;i>=0;i--) //traverse the col of the p_mat downwards.
    {
        if(p[i]&index){
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

    int new_margin_size=current_cursor+1; //compute new margin size for the next layer
    
    if(new_margin_size<=0) //end
    {
        return;
    }
    
    left_or_right=(current_cursor%2)==0;
    for(int i=current_cursor;i>=0;i--) 
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
        free(root);
        return;
    }
    tree_destroyer(root->left);
    tree_destroyer(root->right);
}

/*
use a random walk through the DDG tree to get a sample
*/
int knuth_yao_sampling(uint64_t random_bits, node * tree)
{
    node *current_tree=tree;
    uint64_t index=1;
    for(uint8_t i=0;i<64;i++)
    {
        
        if(current_tree->type!=255){
            return current_tree->type;
        }

        //use one random bit
        if((random_bits&index)&index)
        {
            current_tree=current_tree->right;
        }
        else{
            current_tree=current_tree->left;
        }
        //renew index
        index=index<<1;
    }
}

//convert a floating-point number to its binary form(truncated).
uint32_t float_to_binary(float x,int percision) //x<1
{
    uint32_t acc=0x00000000;
    uint32_t index=0x80000000;
    for(int i=0;i<percision;i++)
    {
        x*=2;
        int flag=(int)x;
        x-=flag;
        if(flag){
            acc|=index;
        }
        index>>=1;
    }
    return acc;
}

//normalization for p_mat
bool make_sum_to_one(float * p_real ,float sum,uint32_t * p,uint8_t current_num,int percision){
    
    uint32_t acc=0x00000000;
    for(uint8_t i =0;i<current_num;i++)
    {
        p_real[i]/=sum;  
        uint32_t tmp= float_to_binary(p_real[i],percision); 
        p[i]=tmp;
        acc+=tmp;
    }
    if(acc){
        //its sum is not one, then we record the difference and store it in p.
        //it is worthwhile to note that this process is necessary for the program to run correctly.
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


