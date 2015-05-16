#!/bin/bash

cd pngfiles
rm *.png 2> /dev/null
cd ..
#dir = 'datfiles'

./plot.sh "datfiles/create-many-pthread.dat" "datfiles/create-many-context.dat" 'create-many.png' "Nombre de threads"
./plot.sh "datfiles/create-many-once-pthread.dat" "datfiles/create-many-once-context.dat" 'create-many-once.png' "Nombre de threads"
./plot.sh "datfiles/create-many-recursive-pthread.dat" "datfiles/create-many-recursive-context.dat" 'create-many-recursive.png' "Nombre de threads"
./plot.sh "datfiles/fibonacci-pthread.dat" "datfiles/fibonacci-context.dat" 'fibonacci.png' "Valeur de l'entrée"
./plot.sh "datfiles/switch-many-pthread.dat" "datfiles/switch-many-context.dat" 'switch-many.png' "Nombre de threads (100 yields)"
./plot.sh "datfiles/switch-many2-pthread.dat" "datfiles/switch-many2-context.dat" 'switch-many2.png' "Nombre de yields (1000 threads)"
./plot.sh "datfiles/switch-many-join2-pthread.dat" "datfiles/switch-many-join2-context.dat" 'switch-many-join2.png' "Nombre de yields (1000 threads)"
./plot.sh "datfiles/switch-many-join-pthread.dat" "datfiles/switch-many-join-context.dat" 'switch-many-join.png' "Nombre de threads (100 yields)"
