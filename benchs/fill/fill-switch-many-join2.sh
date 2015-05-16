#!/bin/bash

echo 'switch-many-join (yield)'

#amout of yields defined by user (3rd parameter)
cd ../datfiles
rm switch-many-join2-pthread.dat switch-many-join2-context.dat 2> /dev/null
cd ../..

NBTHREAD=${1:-1000}
PTHREAD="tests/build/32-switch-many-join-pthread"
CONTEXT="tests/build/32-switch-many-join-context"

for ((i = 1000000; i >= 0; i -= 20000))
do
    START=$(date +%s%3N)
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/build" $PTHREAD $NBTHREAD $i >/dev/null
    END=$(date +%s%3N)
    DIFF=$(( $END - $START ))
    echo $i $DIFF >> "benchs/datfiles/switch-many-join2-pthread.dat"
    START2=$(date +%s%3N)
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/build" $CONTEXT $NBTHREAD $i >/dev/null
    END2=$(date +%s%3N)
    DIFF2=$(( $END2 - $START2 ))
    echo $i $DIFF2 >> "benchs/datfiles/switch-many-join2-context.dat"
done
