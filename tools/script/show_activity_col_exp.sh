#!/bin/bash

head -2000 "$2/o_$1.txt" | grep . > out_tmp
touch plot
if [ $# != 2 ]
then 
echo "set dgrid3d 50,50,1" >> plot
echo "set pm3d map interpolate 10,10" >> plot
fi
echo "splot 'out_tmp'" >> plot
gnuplot -persist plot
rm out_tmp plot

