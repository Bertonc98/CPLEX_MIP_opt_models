#!/bin/bash
#Call by specifying the model name as first parameter
for i in {1..10}
	do for j in 4 8
		do for k in 3 9
			do ./$1 "../src/data/basic_models/basic_model"$i"_"$j"_"$k".lp" 1 1 "../src/parameters/all.txt"
		done;
	done;
done;
