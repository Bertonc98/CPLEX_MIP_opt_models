import subprocess as sp

with open("../src/data/basic_models/results.txt") as f:
	res = f.readlines()
	
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

keys = ["../src/parameters/"+k+".txt" for k in thresholds.keys()]

keys.append("../src/parameters/all.txt")

results = {k : [] for k in keys}
for line in res:
	l = line.strip()
	if len(l) > 2 and l[:2] == "..":
		save_key = l
	else:
		results[save_key].append(l)

for k, txt in results.items():
	save_path = "../src/data/basic_models/results_"+k.split("/")[-1]
	with open(save_path, "w+") as sf:
		sf.writelines("\n".join(txt))
	sf.close()

	
