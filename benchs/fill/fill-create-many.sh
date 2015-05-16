#!/bin/bash

echo create-many

cd ../datfiles
rm create-many-pthread.dat create-many-context.dat 2> /dev/null
cd ../..

PTHREAD="tests/build/21-create-many-pthread"
CONTEXT="tests/build/21-create-many-context"

for ((i = 10000; i >= 0; i -= 100))
do
    START=$(date +%s%3N)
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/build" $PTHREAD $i >/dev/null
    END=$(date +%s%3N)
    DIFF=$(( $END - $START ))
    echo $i $DIFF >> "benchs/datfiles/create-many-pthread.dat"
    START2=$(date +%s%3N)
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/build" $CONTEXT $i >/dev/null
    END2=$(date +%s%3N)
    DIFF2=$(( $END2 - $START2 ))
    echo $i $DIFF2 >> "benchs/datfiles/create-many-context.dat"
done
