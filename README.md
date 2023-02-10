# CPLEX_MIP_opt_models
In order to compile the two models:

- Move to exe directory
- Execute "./maker basic_model/strong_model 0", this will compile the model file
- Execute the model as: "basicmodel/strong_model instance d k"
  Where instance is the toy instance number, d and k as defined in the model
  
The basic model output comprehend the comparison between the provided results and the computed ones on the bash\\
The strong model does not solve any instance (infeasible)

# Project structure
CPLEX_MIP_opt_models <br />
├── exe <br />
│   └── blueprint <br />
└── src <br />
    ├── data <br />
    │   └── basic_models <br />
    │   └── stronger_models <br />
    ├── instance_set <br />
    │   └── optimal_solutions <br />
    └── parameters <br />
         └── basic_model <br />
         └── stronger_model <br />


## 06/11/2022 Update
Run the models over each instance and compared the obj value results with the provided ones

Basic model:\\
The differences seems to be small, but for some instances there are mismatched results

Stronger model:\\
For some instances the resolution of the problem seems to be unfeasible, we tried to check the model code

The results are in the folder ./exe/basic_result.csv and ./exe/strong_result.csv\\
The code for the two models are in the ./src folder

## 07/11/2022 update
Exploited the .c file to extract cuts from the branch and cut resolution. \\
The cuts are saved into src/data/ as node0.lp file, the file contains the whole problem with new constraint in the "bounds" section 

The file for the cut_extraction is in the exe directory.\\
In order to compile this .c file there is bash script into exe/blueptint directory\\
This is used by the maker program.\\
In order to compile the cut_extraction file the maker can be called by: ./maker cut 1

The executable program must be called as:\\
./cut_extraction model_name.lp 1 1 *parameter_file\\
Where 1 1 indicates that the cuts are saved only from the root node
*parameter_file optional

In order to compile a model file: ./maker $name_of_the_model 0\\
The program will save the model of the problem into the src/data directory with the same neame of the problem

## 13/11/2022 add
Tower problem

## 14/11/2022 update
In order to generate more cuts a "parameters" folder has been created in src/
The files in this folder can be passed o the "cut_extraction" program, as third parameter, to
set the value for the generation of the cuts (in all.txt) there are all the parameters with the higher possible value (aggressive/very aggressive)

It is possible create different configuration files, with even different parameters other than the cut generation ones.

The values of the cuts parameters are:
- -1: no generation
- 0: default generation (auto adapt the number)
- 1: moderate generation
- 2: aggressive generation
- 3: very aggressive generation (onlt for CPXPARAM_MIP_Cuts_LocalImplied, CPXPARAM_MIP_Cuts_Cliques, CPXPARAM_MIP_Cuts_Covers, CPXPARAM_MIP_Cuts_Disjunctive, CPXPARAM_MIP_Cuts_LiftProj)

## 28/11/2022 update
Created new folder to save basic_model lp for each different instance
The model will be exported with the instance number, d and k parameters in the name
The whole results of the runs are saved into a "results.txt" in the same folder
A file "format_result.py" has been added in order to extract the informations about the 
cuts and the relatives informations.

A file "run_extraction.sh" has been added to run all the tests and save the output into "results.txt"
To save new results an example:
bash run_extraction_result.sh cut_extraction > ../src/data/basic_models/results.txt

## 29/11/2022
Created search_gap.py that parse the results obtained from the cut_extraction in order to have formatted data saved as
"formatted_results.txt"
The parameter_geenration.py creates all the possible combination of parameters for cut generation with only one active
Modified run_extraction.sh in order to apply for each instance the whole set of possible parameters file (genereted by the previous py file)
Some minor modifies in the codes in order to better parse the output
Added split_results.py to divide the output of the overall extraction

The flow will be:
- bash run_tests_models.sh basic_model -> save all the models on the different instances
- bash run_extraction_results.sh cut_extraction >> ../src/data/basic_models/results.txt -> obtain the results of each model and each instance with every single parameter active  
- split_results.py -> divide results.txt from the previous point in different files according to the parameter that generated them
- format_result.py -> must obtain the formatted results and search for best gap (and save number of cuts)
- gap_extraction.sh -> retrieve a single file for each cut containing the gaps and the number of cuts applied in each instance

## 04/12/2022 
Improved the generality of the script for each model and provided a single script to run the whole process "gap_pipeline.sh"
must be called passing the name of the model that is under analysis and retrieve the results in the src/data/model_names/ folder

## 05/12/2022
gap_pipeline compelted, even on stronger_model results.
New directory to divide basic/stronger files (see dir tree)

## 15/12/2022
Quadratic model implementation, test and plot

## 03/02/2023
Implementation of linearized model and first tests over the cut extraction

## 04/02/2023
Add DUMB configuration to generate cuts in linearized model

## 05/02/2022
Parsing file to export cuts from lp file "node_aggressive0.lp" in "export_cuts.cpp"
It creates a new file "constraints.txt" in which are contained only the new constraints
Pipeline to extract cuts from linearized model:

./make linearized_model 0
./linearized_model n d k k
./cut_extraction ../src/data/linearized_models/linearized_model1_4_3.lp 0 1 ../src/parameters/linearized_model/CPXPARAM_MIP_Cuts_DUMB.txt
python3 generate_G.py

### Gamma matrix desctiprion:
It contains for each constraint a line that is composed by the coefficients of the variables, in order of (w, z, pp, pm, f, s)
Defining as "d" the amount of features, and as "k" the amount of points there are relatively (d + 1 + k + k + d + k) entries in each row
So in each column there are the coefficients of a particular variable in each constraint

## 09/02/2023
Some minor error in the definition of the dual fixed
Added handlingo of case in which no constraint is generated and GAMMA would be empty

TODO: 
- Check for feasibility of the dual


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

