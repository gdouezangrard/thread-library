#!/bin/bash

dir=`dirname $BASH_SOURCE`

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$dir/../build"

mkdir -p "$dir/build"

shopt -s nullglob

for f in $dir/*.c
do
    printf "Compiling $f..."
    for l in 'context' 'pthread'
    do
        printf "\n"
        fn="${f##*/}"
        fn="${fn%.*}"
        if [ "$l" == 'pthread' ]; then
            gcc -DUSE_PTHREAD -pthread -I"$dir/../src" -L"$dir/../build" -l"os$l" -o "$dir/build/$fn-$l" $f
        else
            if [ "$1" == 'release' ]; then
                gcc -DNDEBUG -I"$dir/../src" -L"$dir/../build" -l"os$l" -o "$dir/build/$fn-$l" $f
            else
                gcc -I"$dir/../src" -L"$dir/../build" -l"os$l" -o "$dir/build/$fn-$l" $f
            fi
        fi
        printf "OK $dir/build/$fn-$l"
    done
    printf "\n"
done
