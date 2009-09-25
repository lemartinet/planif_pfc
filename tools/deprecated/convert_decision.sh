#!/bin/bash

# utilisation : convert_decision.sh experience_dir (e.g. ../../data/data_raw/1/)

sed -e "s:planning:1:g" $1/decision.txt | sed -e "s:exploration:0:g" | sed -e "s:no_action_exploration:-0:g" > $1/decision2.txt
mv $1/decision2.txt $1/decision.txt

