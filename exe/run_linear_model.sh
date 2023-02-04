#!/bin/bash
#Call by specifying the model name as first parameter
# The amount of outliers is fixed at 1
for i in {1..10}
	do for j in 4 8
		do for k in 3 9
			do ./$1 $i $j $k $k
		done;
	done;
done;
