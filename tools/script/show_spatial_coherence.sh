#!/bin/sh

# probleme : on compte aussi les moments ou way1 est ouvert...
# pb2 : on verifie seulement que le premier choix est way2, 
# mais pas qu'il mene au but (le robot pour aller dans -way3)

nb_days=14
nb_trial=12

for exp in `ls -d */` 
do
if [ $# != 0 ]
then
echo "Exp $exp"
fi
training=`cat $exp/output_supervisor.txt | grep -v "Day 15" | grep -v "\-way*"`
cpt_exp=0
	for day in `seq 2 $nb_days`
	do
	cpt_day=0
	training_day=`echo "$training" | grep "Day $day "`
		for i in `seq 1 $nb_trial`
		do
		val2=1
		line=2
		cpt=-1
		training_trial=`echo "$training_day" | grep "Trial $i "`
			while [ $val2 != "way2" ] && [ $val2 != "goal" ]
			do
			val1=`echo "$training_trial" | head -$line | tail -1`
			val2=`echo $val1 | cut -d":" -f2 | cut -d" " -f2`
			line=$(( $line + 1 ))
			cpt=$(( $cpt + 1 ))
			done
		cpt_day=$(( $cpt_day + $cpt))
		echo -n $cpt
		echo -n " "
		done
	cpt_exp=$(( $cpt_exp + $cpt_day))
	mean=`echo "scale=2 ; $cpt_day / $nb_trial" | bc`
	echo ": $cpt_day : $mean"
	done
nb_days=$(( $nb_days - 1 ))
mean=`echo "scale=2 ; $cpt_exp / ($nb_trial * $nb_days)" | bc`
echo "Total : $cpt_exp : $mean"
echo
done
