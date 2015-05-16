#!/bin/bash

dir=`dirname $BASH_SOURCE`

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$dir/../build"

mkdir -p "$dir/build"

shopt -s nullglob

for f in $dir/*.c
do
    printf "Running $f..."
    for l in 'context' 'pthread'
    do
        printf "\n"
        fn="${f##*/}"
        fn="${fn%.*}"
        "./$dir/build/$fn-$l"
    done
done
