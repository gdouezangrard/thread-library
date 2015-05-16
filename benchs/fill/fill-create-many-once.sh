#!/bin/bash

echo create-many-once

cd ../datfiles
rm create-many-once-pthread.dat create-many-once-context.dat 2> /dev/null
cd ../..

PTHREAD="tests/build/23-create-many-once-pthread"
CONTEXT="tests/build/23-create-many-once-context"

for ((i = 10000; i >= 0; i -= 100))
do
    START=$(date +%s%3N)
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/build" $PTHREAD $i >/dev/null
    END=$(date +%s%3N)
    DIFF=$(( $END - $START ))
    echo $i $DIFF >> "benchs/datfiles/create-many-once-pthread.dat"
    START2=$(date +%s%3N)
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/build" $CONTEXT $i >/dev/null
    END2=$(date +%s%3N)
    DIFF2=$(( $END2 - $START2 ))
    echo $i $DIFF2 >> "benchs/datfiles/create-many-once-context.dat"
done
