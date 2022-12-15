import subprocess as sb
import itertools
import sys

if len(sys.argv) != 2:
	print("Use as python3 gap_search model_name\n\n")
	exit()

model_name = sys.argv[1]

stuff = ["CPXPARAM_MIP_Cuts_"+cut for cut in ["Gomory", "Disjunctive", "LiftProj"]]
subsets = []
for L in range(1, len(stuff) + 1):
    for subset in itertools.combinations(stuff, L):
        subsets.append( "-".join(subset) )

thresholds = {
"CPXPARAM_MIP_Cuts_LocalImplied": 3,
"CPXPARAM_MIP_Cuts_Cliques":	3,
"CPXPARAM_MIP_Cuts_Covers":	3,
"CPXPARAM_MIP_Cuts_Disjunctive":	3,
"CPXPARAM_MIP_Cuts_LiftProj":	3,
"CPXPARAM_MIP_Cuts_Implied":	2,
"CPXPARAM_MIP_Cuts_BQP":		2,
"CPXPARAM_MIP_Cuts_FlowCovers":	2,
"CPXPARAM_MIP_Cuts_PathCut":	2,
"CPXPARAM_MIP_Cuts_Gomory":	2,
"CPXPARAM_MIP_Cuts_GUBCovers":	2,
"CPXPARAM_MIP_Cuts_MIRCut":	2,
"CPXPARAM_MIP_Cuts_RLT":		2,
"CPXPARAM_MIP_Cuts_ZeroHalfCut":	2,
"CPXPARAM_MIP_Cuts_MCFCut":	2
}

for sub in subsets:
	text = "CPLEX Parameter File Version 22.1.0.0\n"
	active_params = sub.split("-")
	for k in thresholds.keys():
		if k in active_params:
			text += f"{k} {thresholds[k]}\n"
		else:
			text += f"{k} -1\n"
	
	name = "-".join([c.split("_")[-1] for c in active_params])
	
	with open(f"../src/parameters/{model_name}/CPXPARAM_MIP_Cuts_{name}.txt", "w+") as f:
		f.writelines(text)
	f.close()
