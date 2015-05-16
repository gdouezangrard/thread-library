#!/bin/bash

gnuplot << EOF

set terminal png
set logscale y
set title "Test de performances"
set xlabel "$4"
set ylabel "Temps d'exécution (ms)"
set autoscale
set output 'pngfiles/$3'


plot 	   "$1" using 1:2 title 'pthread' with linespoints, \
	   "$2" using 1:2 title 'context' with linespoints

EOF
