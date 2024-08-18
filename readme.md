运行实验：

```
cd Integer_Gaussian_sampler
clang -o  test test_sampler.c sampler.c Improved_Karney.c  basic_compute.c ../example/sampler/rng.c ../example/sampler/shake.c -mavx -mavx2 -mfma
./test
```

测试代码使用内存开销
```command time -v 绝对路径/file_name```

Normality test command
```
cd normality_test
python normality_test.py
python normality_verify_fig.py #画图
```