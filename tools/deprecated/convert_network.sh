#!/bin/bash

# utilisation : convert_network.sh experience_dir (e.g. ../../data/data_raw/1/)

sed -e "s:wrong:0:g" $1/network.txt | sed -e "s:update_minicol:1:g" | sed -e "s:new_minicol:2:g" | sed -e "s:best_minicol:3:g" | sed -e "s:->: :g" | sed -e "s:\:: :g" > $1/network2.txt
mv $1/network2.txt $1/network.txt

