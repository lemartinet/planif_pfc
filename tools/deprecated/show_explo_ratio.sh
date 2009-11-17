#!/bin/sh

nb_day=15

for exp in `ls -d */` 
do
if [ $# != 0 ]
then
echo -n "Exp $exp : "
fi
	for i in `seq 1 $nb_day`
	do
	explo=`cat $exp/output_mypuck.txt | grep "Day $i " | grep explo | wc -l`
	planif=`cat $exp/output_mypuck.txt | grep "Day $i " | grep -v explo | grep -v goal | grep -v pas | wc -l`
	ratio=`echo "scale=2 ; $explo / ($explo + $planif)" | bc`
	echo -n "$ratio "
	done
echo
done

