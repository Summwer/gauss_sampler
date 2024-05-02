cd Karney
gcc -o karney_prng my_Karney_prng.c speed_print.c cpucycles.c shake.c rng.c -lm 
command time -v ~/lattice-experiments/gauss_sampler/Karney/karney_prng 2 -1. 1.7 0 1> karney_prng_test_res.rsp
./karney_prng 2 -1. 1.7 1
cd ..

cd example/sampler
gcc -o example_test_res  main.c fpr.c rng.c sampler.c cpucycles.c shake.c speed_print.c 
command time -v ~/lattice-experiments/gauss_sampler/example/sampler/example_test_res   1> example_test_res.rsp
./example_test_res 





