#!/bin/sh

# affichage des résultats en animation

imax=`grep "#" L1 | cut -f2 -d" "`

echo "plot 'L1' index i w p pt 6
i=i+1
pause 1
if (i>=$imax) i=0 
reread" > boucle1.gnuplot


echo "plot 'L2' index i w p pt 6
i=i+1
pause 1
if (i>=$imax) i=0 
reread" > boucle2.gnuplot


echo "plot 'L3' index i w p pt 6
i=i+1
pause 1
if (i>=$imax) i=0 
reread" > boucle3.gnuplot


echo "set hidden3d
splot 'LH' index i w l palette
i=i+1
pause 1
if (i>=$imax) i=0 
reread" > boucleH.gnuplot


echo "i=0
load 'boucle1.gnuplot'" | gnuplot & 

echo "i=0
load 'boucle2.gnuplot'" | gnuplot &

echo "i=0
load 'boucle3.gnuplot'" | gnuplot &

echo "i=0
load 'boucleH.gnuplot'" | gnuplot &

echo "plot 'ESTIM' u 1 w l, 'ESTIM' u 2 w l, 'ESTIM' u 3 w l, 'ESTIM' u 4 w l" | gnuplot -persist &

read

rm boucle*
killall gnuplot gnuplot_x11
