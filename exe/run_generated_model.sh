#!/bin/bash
#Call by specifying the model name as first parameter
for i in {1..5}
	do for k in 50 100 150
		do 
		p=$((k/10))
		./$1 $i 4 $p $k
	done;
done;
