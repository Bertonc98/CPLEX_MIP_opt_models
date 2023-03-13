#!/bin/bash
#Call by specifying the model name as first parameter
for i in 1
	do for k in 50 100 150
		do for d in 10 20 30
			do for w in 10 50 100
				do
				echo "RUNNING: i: " $i " k: " $k " d: " $d " w: " $w	
				p=$((k/10))
				./$1 $i 4 $p $k $d $w
			done;
		done;
	done;
done;
