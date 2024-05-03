cd Karney
echo "Karney:"
gcc -o karney my_Karney_prng.c speed_print.c cpucycles.c shake.c rng.c -lm 
./karney 1
cd ..

cd example/sampler
echo "example:"
gcc -o example_test_res main.c fpr.c rng.c sampler.c cpucycles.c shake.c speed_print.c  
./example_test_res 1
cd .. 
cd .. 


cd pre17
echo "pre17:"
gcc -o main.o main.c fpr.c rng.c sampler.c cpucycles.c shake.c speed_print.c 
./main.o 1
cd .. 


cd Knuth-Yao
echo "Knuth-Yao:"
gcc -o knuth_yao knuth_yao.c -lm
./knuth_yao 1
cd .. 


cd normality_test
python normality_test.py 