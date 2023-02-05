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

with open("../src/data/constraints.txt") as f:
	lines = [line.strip() for line in f]
	
with open("../src/data/feature_point.txt") as f:
	d = int(f.readline().strip())
	k = int(f.readline().strip())
# w, z, pp, pm, f, s
# d + 1 + k + k + d + k
n_var = d + 1 + k + k + d + k
matrix = []
for l in lines:
	m_line = [0 for _ in range(n_var)]
	constraint = l.split("<=")[0]
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

for m in matrix:
	print(m)
	print("")
