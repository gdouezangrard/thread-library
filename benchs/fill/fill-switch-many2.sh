#!/bin/bash

echo 'switch-many (yields)'

#amout of yields defined by user (3rd parameter)
cd ../datfiles
rm switch-many2-pthread.dat switch-many2-context.dat 2> /dev/null
cd ../..

NBTHREAD=${1:-1000}
PTHREAD="tests/build/31-switch-many-pthread"
CONTEXT="tests/build/31-switch-many-context"

for ((i = 2000; i >= 0; i -= 100))
do
    START=$(date +%s%3N)
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/build" $PTHREAD $NBTHREAD $i >/dev/null
    END=$(date +%s%3N)
    DIFF=$(( $END - $START ))
    echo $i $DIFF >> "benchs/datfiles/switch-many2-pthread.dat"
    START2=$(date +%s%3N)
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/build" $CONTEXT $NBTHREAD $i >/dev/null
    END2=$(date +%s%3N)
    DIFF2=$(( $END2 - $START2 ))
    echo $i $DIFF2 >> "benchs/datfiles/switch-many2-context.dat"
done
