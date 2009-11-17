#!/bin/sh

nb_trial=7

for exp in `ls -d */` 
do
if [ $# != 0 ]
then
echo -n "Exp $exp : "
fi
test=`cat $exp/output_supervisor.txt |grep "Day 15 " | grep -v "\-way*"`
	for i in `seq 1 $nb_trial`
	do
	val1=`echo "$test" | grep "Trial $i " | head -2 | tail -1`
	val2=`echo $val1 | cut -d":" -f2 | cut -d"y" -f2`
	echo -n $val2
	if [ $val2 != 3 ]
	then
	echo -n "/"
	val1=`echo "$test" | grep "Trial $i " | head -3 | tail -1`
	val2=`echo $val1 | cut -d":" -f2 | cut -d"y" -f2`
	echo -n $val2
	fi
	if [ $val2 != 3 ]
	then
	echo -n "/"
	val1=`echo "$test" | grep "Trial $i " | head -4 | tail -1`
	val2=`echo $val1 | cut -d":" -f2 | cut -d"y" -f2`
	echo -n $val2
	fi
	echo -n " "
	done
echo
done

