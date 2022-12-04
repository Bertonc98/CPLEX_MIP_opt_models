#!/bin/bash
#Call by specifying the model name as first parameter

for f in ../src/data/basic_models/formatted*.txt; 
	do
		name=$(awk -F "/" '{print $5}' <<< $f)
		echo $name
		cat $f | awk '{if (NF == 1) print "Instance"$0; else if (NF == 2) print $0; else print $8}' > "../src/data/basic_models/gap_"$name
	done
