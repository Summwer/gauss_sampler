cd Karney
gcc -o karney my_Karney_prng.c speed_print.c cpucycles.c shake.c rng.c -lm 
command time -v ~/lattice-experiments/gauss_sampler/Karney/karney 0

gcc -o karney_prng my_Karney_prng.c speed_print.c cpucycles.c shake.c rng.c -lm 
command time -v ~/lattice-experiments/gauss_sampler/Karney/karney_prng 0
cd ..

cd example/sampler
gcc -o example_test_res  main.c fpr.c rng.c sampler.c cpucycles.c shake.c speed_print.c 
command time -v ~/lattice-experiments/gauss_sampler/example/sampler/example_test_res 0   
cd .. 
cd .. 


cd pre17
gcc -o main.o main.c fpr.c rng.c sampler.c cpucycles.c shake.c speed_print.c 
command time -v ~/lattice-experiments/gauss_sampler/pre17/main.o 0
cd .. 


cd Knuth-Yao
gcc -o knuth_yao knuth_yao.c -lm
command time -v ~/lattice-experiments/gauss_sampler/Knuth-Yao/knuth_yao 0 
cd .. 


