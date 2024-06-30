#!/bin/bash

CC=clang

CFLAGS="-mavx2"

LDFLAGS="-lm"

SOURCE_FILE="test_sampler.c sampler_knuthyao.c knuth_yao.c ../example/sampler/rng.c ../example/sampler/shake.c"

SOURCE_FILE=test_sampler.c sampler.c my_improved_Karney_prng2.c ../example/sampler/rng.c ../example/sampler/shake.c  -lm
OUTPUT_FILE="test"

$CC $CFLAGS $LDFLAGS $SOURCE_FILE -o $OUTPUT_FILE
