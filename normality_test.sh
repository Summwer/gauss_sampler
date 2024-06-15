cd Karney
echo "Karney:"
clang -o karney my_Karney_prng.c speed_print.c cpucycles.c shake.c rng.c -lm 
# clang -o karney my_Karney.c speed_print.c cpucycles.c shake.c rng.c -lm 
./karney 1
cd ..

cd example/sampler
echo "example:"
clang -o example_test_res main.c fpr.c rng.c sampler.c cpucycles.c shake.c speed_print.c  
./example_test_res 1
cd .. 
cd .. 


cd pre17
echo "pre17:"
clang -o main.o main.c fpr.c rng.c sampler.c cpucycles.c shake.c speed_print.c 
./main.o 1
cd .. 


cd Knuth-Yao
echo "Knuth-Yao:"
clang -o knuth_yao_org knuth_yao_org.c -lm
./knuth_yao_org
cd .. 


cd normality_test
python normality_test.py 