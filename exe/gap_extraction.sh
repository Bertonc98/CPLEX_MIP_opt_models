#!/bin/bash
#Call by specifying the model name as first parameter

for f in ../src/data/$1s/formatted*.txt; 
	do
		name=$(awk -F "/" '{print $5}' <<< $f)
		cat $f | awk '{if (NF == 1) print "Instance"$0; else if (NF == 2) print $0; else print $8}' > "../src/data/"$1"s/gap_"$name
	done
