#!/bin/bash
#Call by specifying the cut_extraction as first parameter and the model name as second parameter
for i in {1..10}
	do for j in 4 8
		do for k in 3 9
			do for f in ../src/parameters/$2/*
				do 
				name=$(awk -F "/" '{print $4}' <<< $f)
				name=$(awk -F "." '{print $1}' <<< $name)
				sudo cp "../src/data/"$2"s/"$2$i"_"$j"_"$k".lp" "../src/data/"$2"s/"$2$i"_"$j"_"$k"_"$name".lp"
				sudo ./$1 "../src/data/"$2"s/"$2$i"_"$j"_"$k"_"$name".lp" 1 1 $f
			done;
		done;
	done;
done;
