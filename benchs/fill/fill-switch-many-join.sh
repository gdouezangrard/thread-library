#!/bin/bash

echo switch-many-join

#amout of yields defined by user (3rd parameter)
cd ../datfiles
rm switch-many-join-pthread.dat switch-many-join-context.dat 2> /dev/null
cd ../..

NBYIELD=${1:-100}
PTHREAD="tests/build/32-switch-many-join-pthread"
CONTEXT="tests/build/32-switch-many-join-context"

for ((i = 10000; i >= 0; i -= 200))
do
    START=$(date +%s%3N)
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/build" $PTHREAD $i $NBYIELD >/dev/null
    END=$(date +%s%3N)
    DIFF=$(( $END - $START ))
    echo $i $DIFF >> "benchs/datfiles/switch-many-join-pthread.dat"
    START2=$(date +%s%3N)
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$(pwd)/build" $CONTEXT $i $NBYIELD >/dev/null
    END2=$(date +%s%3N)
    DIFF2=$(( $END2 - $START2 ))
    echo $i $DIFF2 >> "benchs/datfiles/switch-many-join-context.dat"
done
