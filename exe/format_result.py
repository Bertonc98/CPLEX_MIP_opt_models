import sys
import subprocess as sp

if len(sys.argv) != 2:
	print("Use as python3 gap_search model_name\n\n")
	exit()

model_name = sys.argv[1]
out = sp.run("ls ../src/parameters/".split(), capture_output = True, text = True)
params = [fl.split(".")[0] for fl in out.stdout.strip().split("\n")]

for cut in params:
	active_params = cut.split("/")[-1].split("_")[-1].split(".")[0].split("-")
	n_active_params = len(active_params)
	
	with open("../src/data/"+model_name+"s/results_"+cut+".txt") as f:
		res = f.readlines()

	results = [[]]
	to_save = [[]]
	add = False
	check = False
	header = True
	pos = 0
	for row in res:
		if "Elapsed" in row:
			continue
		if "Detecting" in row:
			check = True
			continue
		if check and row.strip() == "":
			add = False
			check = False
		if "Nodes" in row:
			add = True
		if "cuts applied:" in row:
			add = False
			header = True
			pos += 1
			results.append([])
		if "cuts " in row:
			results[pos-1].append(row)
			continue
		if add:
			if "Objective" in row:
				if header:
					results[pos].append(row)
					header = False
			elif len(row.strip()) > 0:
				results[pos].append(row)
				
	if "BQP" in cut:
		print(results)
	# ~ print(active_params)
	for i in range(0, len(results[:-1]), n_active_params):
		formatted_text = ""
		
		for j in range(n_active_params):
			for row in results[i+j]:
				if "Cuts/" not in row:
					if "cuts " in row:
						pattern = row.replace(" ", "")
						pattern = pattern.split(":")[1]
						if n_active_params > 1:
							formatted_text = formatted_text + cut + ":" + active_params[j] + " " + pattern
						else:
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
			to_save[i//n_active_params] = str(i//n_active_params) + "\n" + formatted_text
			to_save.append([])
			# ~ results[i] = str(i) + "\n" + formatted_text
	

	# ~ print(to_save)	
	with open("../src/data/"+model_name+"s/formatted_"+cut+"_results.txt", "w+") as fr:
		fr.writelines(to_save[:-1])

