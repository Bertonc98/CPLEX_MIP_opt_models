import subprocess as sb

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

for t in thresholds.keys():
	text = "CPLEX Parameter File Version 22.1.0.0\n"
	for tt in thresholds.keys():
		if t != tt:
			text += (f"{tt} -1\n")
		else:
			text += (f"{tt} {thresholds[tt]}\n")
	with open(f"../src/parameters/{t}.txt", "w+") as f:
		f.writelines(text)
	f.close()
