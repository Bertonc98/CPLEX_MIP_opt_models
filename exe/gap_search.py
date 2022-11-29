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

results = [[]]
add = False
check = False
pos = 0
for row in res:
	if check and row.strip() == "":
		add = False
		check = False
	if "Nodes" in row:
		add = True
	if "Elapsed" in row:
		add = False
		pos += 1
		results.append([])
	if "Detecting" in row:
		check = True
		continue
	if add:
		if len(row.strip()) > 0:
			results[pos].append(row)

for i in range(len(results[:-1])):
	formatted_text = ""
	for row in results[i]:
		pattern = row
		pattern = pattern.replace(": ", ":")
		pattern = pattern.replace("Best Bound", "Best_Bound")
		pattern = pattern.replace("Best Integer", "Best_Integer")
		pattern = pattern.split()
		if pattern[0] == "*":
			pattern.pop(0)
		
		if len(pattern) == 5:
			pattern.insert(2, "nan")
			pattern.insert(3, "nan")
			pattern.insert(6, "nan")
			
		if len(pattern) > 8:
			pattern = pattern[:8]
		
		pattern = " ".join(pattern)
		
		pattern = pattern + "\n"
		formatted_text = formatted_text + pattern
	if len(formatted_text) > 0:
		results[i] = str(i) + "\n" + formatted_text
	# ~ print(results[i])
	
with open("../src/data/basic_models/formatted_results.txt", "w+") as fr:
	fr.writelines(results[:-1])
