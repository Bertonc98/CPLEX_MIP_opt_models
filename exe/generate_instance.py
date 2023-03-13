import csv
import numpy as np

def generate_points(k, d):
  '''
  Generate k points from N(0, I), in d dimension
  Params:
    k: int, cardinality of the sample
    d: int, dimensionality of the points, NOT comprehends the intercept
  
  return: np.array containing k np.array of d-dimensional points
  '''
  sample = []
  rng = np.random.default_rng()
  for i in range(k):
    sample.append(rng.normal(0, 1, d-1))
  return np.array(sample)

def compute_y(x, alpha):
  '''
  Compute y with the dot product w*x, with w with 5 1s and other values 0
  Params:
    x: np.array, of cardinality k, and dimensionality d (it contains intercept)
    alpha: int, signal-tonoise ratio (SNR)
  return: np.array containing the y
  '''
  rng = np.random.default_rng()
  d = x.shape[1]
  k = x.shape[0]

  w = np.array([1 if i < min(6, d)  else 0 for i in range(d)])
  sigma = np.sqrt( np.linalg.norm(w, ord=2)/alpha )
  noise = rng.normal(0, sigma, k)
  y = np.array([sum(x_i * w) for x_i in x])
  return y + noise

def corrupt_response(y, mu_r):
  rng = np.random.default_rng()
  k = y.shape[0]
  outlier_mask = ( np.random.random(k) <= 0.1 ).astype(int)
  return y + ( rng.normal(mu_r, 1, k) * outlier_mask )

def corrupt_row(row, mu_a, b):
  '''
  Generate the corruption of a row, modifying ONLY the first 5 values
  '''
  if b == 1:
    rng = np.random.default_rng()
    d = row.shape[0]
    sign_mask = ( ( ( np.random.random(d) <= 0.5 ).astype(int) * 2 ) - 1 )
    # Nullify corruption over the intercept and 5 dimensions
    for i in range(d):
      if i >= 5:
        sign_mask[i] = 0
    
    errors = rng.normal(mu_a, 1, d)
    return row + (errors * sign_mask)

  return row

def corrupt_bad_leverage(x, y, mu_r, mu_a, pi):
  '''
  Corrupt the selected outliers, extracted with probability pi, over row (x) and responses (y)
  '''
  rng = np.random.default_rng()
  k = x.shape[0]
  d = x.shape[1]

  outlier_mask = ( np.random.random(k) <= pi ).astype(int)

  y = y + ( rng.normal(mu_r, 1, k) * outlier_mask )

  x = np.array([corrupt_row(x[i], mu_a, outlier_mask[i]) for i in range(k)])

  return x, y

def save_instance(name, x, y):
  with open(name, "w") as f:
    f.write("[")
    for i in range(k):
      if i < k-1:
        f.write(np.array2string(x[i], separator = ",").replace("\n", "").replace(" ", "")+ ",\n")
      else:
        f.write(np.array2string(x[i], separator = ",").replace("\n", "").replace(" ", ""))
    f.write("]\n")

    f.write(np.array2string(y, separator = ",").replace("\n", "").replace(" ", ""))

ds = [10, 20, 30]
ks = [50, 100, 150]
alpha = 5
mu_r = -10
mu_a = 10
pi = 0.1

for k in ks:
	for d in ds:
		for i in range(2):
			x = generate_points(k, d)
			y = compute_y(x, alpha)

			# y = corrupt_response(y, mu_r)
			x, y = corrupt_bad_leverage(x, y, mu_r, mu_a, pi)
			path = "../src/instance_set/generated_instances/bigm_tune/"
			save_instance(path + "_".join(["bigmtune", str(k), str(d), "-"+str(i+1)]) + ".dat", x, y)

