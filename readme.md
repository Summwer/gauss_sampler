This is a joint effort by Zhiheng Wang and Dr. Wenwen Xia.
We joined a cryptographic competition about generating discrete gaussians efficiently.
There are four tasks which correspond to the contents in ./Integer_Gaussian_sampler/sampler.h .
We denote the two parameters in discrete gaussians over integers to be $\sigma$ and c.

overview:

The first sampler: $\sigma$ is fixed; c is fixed.

The second sampler: $\sigma$ is fixed but large; c is fixed.

The third sampler: $\sigma$ is fixed; c is variable.

The fourth sampler: $\sigma$ is variable, c is variable.


Dr. Wenwen Xia is responsible for writing the codes in the folder "Integer_Gaussian_sampler" which include table-based method,Karney's algorithm and FALCON's sampler. Dr. Xia adapted these algorithms in order to generate Discrete Gaussians more efficiently, since this competition is more concerned with efficiency than security and it is known widely that securely implemented algorithms such as constant-time or isochronous ones are generally less efficient (To gain the ability to resist timing attacks). Zhiheng Wang leveraged the capabilities of parallel computing by using AVX intrinsics and changed the structure of the original codes. Also, Wang adapted the codes in httpd://github.com/reyoung/avx_mathfun to make a function for computing exp(x) when x is a double percision floating-point number. Although in the final stage, we still used the single percision floating point number because we found out it is sufficient to pass the nomality tests. Also, one YMM regester can be used to store 8 SPFP variables than 4 DPFP variables.

How to compile and run：

```
cd Integer_Gaussian_sampler
clang -o  test test_sampler.c sampler.c Improved_Karney.c  basic_compute.c ../example/sampler/rng.c ../example/sampler/shake.c -mavx -mavx2 -mfma
./test
```


Normality test (sanity check for discrete gaussian distribution) command
```
cd normality_test
python normality_test.py
python normality_verify_fig.py #plot 
```

During the competition, Zhiheng Wang implemented the Knuth-Yao algorithm to generate discrete gaussians which can be found in ./gauss_sampler/Knuth-Yao/knuth_yao.c

Please use the command "gcc -o knuth_yao knuth_yao.c -lm" to compile it and then use "./knuth_yao 1" to run it.

Wang also tried to implement discrete ziggurat algorithm which can be found in the folder "./gauss_sampler/ziggurat" during the competition.