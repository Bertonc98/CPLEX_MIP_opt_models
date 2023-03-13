import sys
import subprocess as sp

out = sp.run(("ls").split(), capture_output = True, text = True)

files = [fl for fl in out.stdout.strip().split("\n") if "bigmtune" in fl]

for fl in files:
	with open("./"+fl) as f:
		res = f.readlines()
		
		lines = res[:-1]
		lines = [l.replace("[", "").replace("]", "").replace(",\n", "").replace("\n","") for l in lines]
		x = [[float(el) for el in p.split(",")] for p in lines]
		
		outliers = ""
		n = 1
		o = 0
		for point in x:
			if -1 < sum(point[:5])/5 < 1:
				outliers += f"{n},0\n"
			else:
				o += 1
				outliers += f"{n},1\n"
			n += 1
	with open("./"+fl.split(".")[0]+"Outliers.dat", "w+") as fr:
		fr.writelines(outliers)
