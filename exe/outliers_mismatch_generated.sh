#!/bin/bash
#Call by specifying the model name as first parameter
for i in {1..5}
		do for d in 10 20 30
				do
				echo "RUNNING: i: " $i " k: 100 d: " $d " w: " $w	
				./$1 $i 4 10 100 $d 100
			done;
done;
