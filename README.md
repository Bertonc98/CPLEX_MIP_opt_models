# CPLEX_MIP_opt_models
In order to compile the two models:

- Move to exe directory
- Execute "./maker basic_model/strong_model", this will compile the model file
- Execute the model as: "basicmodel/strong_model instance d k"
  Where instance is the toy instance number, d and k as defined in the model
  
The basic model output comprehend the comparison between the provided results and the computed ones on the bash
The strong model does not solve any instance (infeasible)

=====================================

## 06/11/2022 Update
Run the models over each instance and compared the obj value results with the provided ones

Basic model:
The differences seems to be small, but for some instances there are mismatched results

Stronger model:
For some instances the resolution of the problem seems to be unfeasible, we tried to check the model code

The results are in the folder ./exe/basic_result.csv and ./exe/strong_result.csv
The code for the two models are in the ./src folder

=====================================

07/11/2022 update
Exploited the .c file to extract cuts from the branch and cut resolution. 
The cuts are saved into src/data/ as node0.lp file, the file contains the whole problem with new constraint in the "bounds" section 

The file for the cut_extraction is in the exe directory.
In order to compile this .c file there is bash script into exe/blueptint directory
This is used by the maker program.
In order to compile the cut_extraction file the maker can be called by: ./maker cut 1

In order to compile a model file: ./maker $name_of_the_model 0


Footer
© 2022 GitHub, Inc.
Footer navigation
Terms
Privacy
Security
Status
Docs
Contact GitHub
Pricing

