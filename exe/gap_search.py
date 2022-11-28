with open("../src/data/basic_models/results.txt") as f:
	res = f.readlines()

results = [[]]
add = False
pos = 0
for row in res:
	if "Nodes" in row:
		add = True
	if "Elapsed" in row:
		add = False
		pos += 1
		results.append([])
	if "Detecting" in row:
		continue
	if add:
		results[pos].append(row.split())

for r in results[0]:
	print(r)
