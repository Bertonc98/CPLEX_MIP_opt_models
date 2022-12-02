import sys

if len(sys.argv) != 2:
	print("Use as python3 gap_search model_name\n\n")
	exit()

model_name = sys.argv[1]

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


for cut in thresholds.keys():
	with open("../src/data/"+model_name+"s/results_"+cut+".txt") as f:
		res = f.readlines()

	results = [[]]
	add = False
	check = False
	pos = 0
	# ~ c = 0
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
		if "cuts " in row:
			results[pos-1].append(row)
			# ~ print(f"{c}: {row}")
			# ~ c += 1
			continue
		if "Detecting" in row:
			check = True
			continue
		if add:
			if len(row.strip()) > 0:
				results[pos].append(row)
	
	for i in range(len(results[:-1])):
		formatted_text = ""
		
		for row in results[i]:
			if "Cuts/" not in row:
				if "cuts " in row:
					# ~ print(f"{i}: {row}")
					pattern = row.replace(" ", "")
					# ~ print(pattern)
					pattern = pattern.split(":")[1]
					formatted_text = formatted_text + cut + " " + pattern
				else:
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
		
	with open("../src/data/"+model_name+"s/formatted_"+cut+"_results.txt", "w+") as fr:
		fr.writelines(results[:-1])

