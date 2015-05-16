#!/bin/bash

echo fibonacci

cd ../datfiles
rm fibonacci-pthread.dat fibonacci-context.dat 2> /dev/null
cd ../..

PTHREAD="tests/build/51-fibonacci-pthread"
CONTEXT="tests/build/51-fibonacci-context"

for ((i = 22; i >= 0; i -= 1))
do
    START=$(date +%s%3N)
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/build" $PTHREAD $i >/dev/null
    END=$(date +%s%3N)
    DIFF=$(( $END - $START ))
    echo $i $DIFF >> "benchs/datfiles/fibonacci-pthread.dat"
    START2=$(date +%s%3N)
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/build" $CONTEXT $i >/dev/null
    END2=$(date +%s%3N)
    DIFF2=$(( $END2 - $START2 ))
    echo $i $DIFF2 >> "benchs/datfiles/fibonacci-context.dat"
done
