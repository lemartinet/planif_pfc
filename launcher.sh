#!/bin/bash

# modifier le fichier params_base.txt si besoin
# utilisation : launcher.sh nb_experiments laby(tolman_maze/tolman_maze_big) make
# - make : 1 ou 0 to compile the code or not
# - first last : number of the first and the last experiment
# - laby : type du world a charger

if [ $1 == 1 ]
then
cd controllers/mypuck_supervisor
make clean ; make -j2
cd ../mypuck
make myclean ; make ui
if [ $CLUSTER ]
	then
	make -j2 -f Makefile_cluster
	else
	make -j2
fi
cd ../..
exit
fi

if [ $# != 4 ]
then
echo "usage : launcher.sh make first last laby(tolman/tolman_big)"
exit
fi


for i in `seq $2 $3`
do
echo "Running simulation $i..."
rm -rf data/data_raw/$i 2> /dev/null
mkdir data/data_raw/$i 2> /dev/null
echo SIMULATION_ID $i > data/params.txt
cat data/params_base.txt >> data/params.txt
nice -19 $WEBOTS_HOME/webots --mode=fast worlds/$4.wbt
mv data/params.txt data/data_raw/$i
echo $i >> data/exps
done

# on remet le monde par defaut pour pas ecraser les fichiers
$WEBOTS_HOME/webots $WEBOTS_HOME/projects/default/worlds/new.wbt &
sleep 10
PID=`ps -eo pid,args | grep new.wbt | grep webots | cut -d "/" -f 1`
kill -9 $PID
echo "All computations finiched !"

