import subprocess as sp
import sys

if len(sys.argv) != 2:
	print("Use as python3 gap_search model_name\n\n")
	exit()

model_name = sys.argv[1]

with open("../src/data/" + model_name + "s/results.txt") as f:
	res = f.readlines()
	
out = sp.run(("ls ../src/parameters/"+model_name+"/").split(), capture_output = True, text = True)
keys = [fl.split(".")[0] for fl in out.stdout.strip().split("\n")]

results = {"../src/parameters/"+model_name+"/"+k+".txt" : [] for k in keys}

for line in res:
	l = line.strip()
	if len(l) > 2 and l[:2] == "..":
		save_key = l
	else:
		if l != "all" and "CPXPARAM" not in l:
			results[save_key].append(l)

for k, txt in results.items():
	save_path = "../src/data/" + model_name + "s/results_"+k.split("/")[-1]
	with open(save_path, "w+") as sf:
		sf.writelines("\n".join(txt))
	sf.close()

	
