#!/bin/bash
#Call by specifying the model name as first parameter
for i in {1..5}
		do for d in 10 20 30
				do
				echo "RUNNING: i: " $i " k: 100" " d: " $d " w: 10"	
				p=$((k/10))
				./$1 $i 4 5 50 $d 10
			done;
		done;
