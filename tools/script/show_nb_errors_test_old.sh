#!/bin/bash

nb_trial=7

nb_exp=0
for exp in `ls -d */` 
do
nb_exp=$(( $nb_exp + 1 ))
if [ $# != 0 ]
then
echo -n "Exp $exp : "
fi
test=`cat $exp/output_supervisor.txt | grep "Day 15"| grep -v "\-way*"`
cpt_exp=0
	for i in `seq 1 $nb_trial`
	do
	val2=1
	line=2
	cpt=-1
	test_trial=`echo "$test" | grep "Trial $i "`
		while [ $val2 != "way3" ]
		do
		val1=`echo "$test_trial" | head -$line | tail -1`
		val2=`echo $val1 | cut -d":" -f2`
		line=$(( $line + 1 ))
		cpt=$(( $cpt + 1 ))
		done		
	echo -n $cpt
	echo -n " "
	cpt_trial[$i]=$(( $cpt + $[cpt_trial[$i]] ))
	cpt_exp=$(( $cpt + $cpt_exp ))
	done
mean=`echo "scale=2 ; $cpt_exp / $nb_trial" | bc`
echo ": $cpt_exp : $mean"
done

echo -n "Total : "
cpt_exp=0
for i in `seq 1 $nb_trial`
do
echo -n "$[cpt_trial[$i]] "
cpt_exp=$(( $[cpt_trial[$i]] + $cpt_exp ))
done
mean=`echo "scale=2 ; $cpt_exp / ($nb_trial * $nb_exp)" | bc`
echo ": $cpt_exp : $mean"

