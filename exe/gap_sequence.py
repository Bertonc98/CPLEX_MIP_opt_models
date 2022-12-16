import sys
import subprocess as sp

if len(sys.argv) != 2:
	print("Use as python3 gap_search model_name\n\n")
	exit()

model_name = sys.argv[1]
out = sp.run(("ls ../src/data/"+model_name+"s/").split(), capture_output = True, text = True)

filenames = ["../src/data/"+model_name+"s/"+fl for fl in out.stdout.strip().split("\n") if "gap_" in fl]

for fl in filenames:
	with open(fl, "r") as f:
		text = f.readlines()
	f.close()
	
	out_text = []
	first = True
	initial_gap = 0.0
	n_cuts = 0
	# print(fl)
	for line in text:
		if "Instance" in line:
			# ~ print(line)
			out_text.append([])
			n_cuts = 0
			out_text[-1].append(line.strip())
			first = True
		elif ":" not in line and first and "Objective" not in line and ";" in line:
			first = False
			obj, bi, _ = line.split(";")
			if "integral" in obj or "cutoff" in obj:
				continue
			
			initial_gap = (float(bi) - float(obj))/float(bi)
		elif ":" in line and "CPX" not in line:		
			added = True
			obj, bi, n_c = line.split(";")
			bi = bi.split(":")[-1]
			if "integral" in obj or "cutoff" in obj:
				continue
			
			# ~ print(initial_gap, obj, bi, n_c, sep="\t\t")
				
			if initial_gap == 0:
				gap = 0
			else:
				gap = (float(bi) - float(obj))/float(bi)
				gap = (initial_gap - gap)/initial_gap
			
			n_cuts += int(n_c.split(":")[-1])
			out_text[-1].append(";".join([str(n_cuts), str(gap)]))
	
	to_file_text = []
	for instance in out_text:
		if len(instance) > 1:
			for el in instance:
				to_file_text.append(el)
	to_file_text.append("\n")
	with open(fl, "w") as f:
		f.write("\n".join(to_file_text))
	f.close()
	#print(to_file_text.flatten())
			
	
			
		
			
	
	
