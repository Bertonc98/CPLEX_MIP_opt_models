def parse_variable(var: list, d, k):
	# w, z, pp, pm, f, s
	# d + 1 + k + k + d + k
	translate = {"w" : 0,
				 "z" : d,
				 "pp" : d + 1,
				 "pm" : d + 1 + k,
				 "f" : d + 1 + k + k,
				 "s" : d + 1 + k + k + d
				 }
				 
	if var[0] == "+":
		if len(var) == 3:
			var = var[1:]
		else:
			var[0] = "+1.0"
	else:
		if len(var) == 3:
			var = var[1:]
			var[0] = "-" + var[0]
		else:
			var[0] = "-1.0"
	if var[1][0] != "z":
		v = var[1][0:var[1].find("(")]
		position = translate[v] + int(var[1][var[1].find("(")+1:var[1].find(")")])
	else:
		position = translate["z"]
	
	return position, var[0]

import subprocess
process = subprocess.Popen(["./export_cuts"])
output, error = process.communicate()
found_constraint = process.returncode
with open("../src/data/feature_point.txt") as f:
	d = int(f.readline().strip())
	k = int(f.readline().strip())
	f.close()
	# w, z, pp, pm, f, s
	# d + 1 + k + k + d + k
n_var = d + 1 + k + k + d + k

# Case of no generated constraints
if(found_constraint == 1):
	print("No generated constraints found, generating zero GAMMA")
	with open("../src/data/GAMMA.txt", 'w') as f:
		l = "[["
		for _ in range(n_var):
			l += "0, "
		l = l[:-2]
		l += "]]\n"
		f.write(l)
		
		f.write("[n]\n")
		
		f.write("1" + "\n")
		
		f.write("0")
else:
	with open("../src/data/constraints.txt") as f:
		lines = [line.strip() for line in f]
		
	
	matrix = []
	coeffs = []
	for l in lines:
		m_line = [0 for _ in range(n_var)]
		constraint = l.split("<=")[0]
		coeffs.append(float(l.split("<=")[1].strip()))
		
		constraint = list(filter(lambda x: x !='', constraint.split(" ")))[1:]

		elements = []
		current = []
		for el in constraint:
			if el in ["-", "+"]:
				elements.append(current)
				current = [el]
			else:
				current.append(el)
		elements.append(current)
		elements.pop(0)
		# print(elements)
		for el in elements:
			p, coeff = parse_variable(el, int(d), int(k))
			m_line[p] = float(coeff)
		matrix.append(m_line)

	with open("../src/data/GAMMA.txt", 'w') as f:
		l = "["
		for row in matrix:
			l += "["
			for el in row:
				l += str(el) + ", "
			l = l[:-2]
			l += "],\n"
		l = l[:-2]
		l += "]\n"
		f.write(l)
		f.write(str(coeffs) + "\n")
		f.write(str(len(matrix)) + "\n")
		f.write("1")
