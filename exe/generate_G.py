import subprocess
process = subprocess.Popen(["./export_cuts"])
output, error = process.communicate()
