#!/bin/sh

nb_day=15

for exp in `ls -d */` 
do
if [ $# != 0 ]
then
echo -n "Exp $exp :"
fi
val=`cat $exp/output_supervisor.txt | grep Test | cut -d":" -f2`
way1=`echo $val | cut -d"," -f1 | cut -d" " -f2`
way2=`echo $val | cut -d"," -f2 | cut -d" " -f3`
way3=`echo $val | cut -d"," -f3 | cut -d" " -f3`
echo "$way1 $way2 $way3"
done

