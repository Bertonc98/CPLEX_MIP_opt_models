#!/bin/bash
#Call by specifying the model name as first parameter
bash run_tests_model.sh $1
bash run_extraction_result.sh cut_extraction $1 > ../src/data/$1s/results.txt
python3 split_results.py $1
python3 format_result.py $1
bash gap_extraction.sh $1
python3 gap_sequence.py $1
