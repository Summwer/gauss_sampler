运行样例：

```
cd example 
clang -o main.o main.c fpr.c rng.c sampler.c cpucycles.c shake.c speed_print.c 
./main.o
```

测试代码使用内存开销
```command time -v 绝对路径/file_name```

Normality test command
```
cd SAGA/code
python 
import saga
mu = 0.
sigma = 1.7
data = [-1, -1, 1, 1, 1, -1] #the concrete data 
res = saga.UnivariateSamples(mu, sigma, data)
res
```