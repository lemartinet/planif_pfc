#!/bin/sh

nb_trial=7

way1=0
way2=0
way3=0

for exp in `ls -d */` 
do
if [ $# != 0 ]
then
echo -n "Exp $exp : "
fi
test=`cat $exp/output_supervisor.txt | grep "Day 15" | grep -v "\-way*"`
cpt_way1=0
cpt_way2=0
cpt_way3=0
	for i in `seq 1 $nb_trial`
	do
	val1=`echo "$test" | grep "Trial $i " | head -2 | tail -1`
	val2=`echo $val1 | cut -d":" -f2 | cut -d"y" -f2`
	echo -n $val2
	echo -n " "
	if [ $val2 = "1" ]
	then
	cpt_way1=$(( $cpt_way1 + 1 ))
	elif [ $val2 = "2" ]
	then
	cpt_way2=$(( $cpt_way2 + 1 ))
	elif [ $val2 = "3" ]
	then
	cpt_way3=$(( $cpt_way3 + 1 ))
	fi
	done
echo ": $cpt_way1 $cpt_way2 $cpt_way3"
way1=$(( $cpt_way1 + $way1 ))
way2=$(( $cpt_way2 + $way2 ))
way3=$(( $cpt_way3 + $way3 ))
done
echo "Total : $way1 $way2 $way3"

