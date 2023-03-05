#ifndef UTILS_H   // To make sure you don't declare the function more than once by including the header multiple times.
#define UTILS_H

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ilcplex/ilocplex.h>

void compute_W(IloNumArray solution, IloNumArray wl, IloNumArray wu, int scale_factor);

void compute_W_optimal_hyperplane(IloNumArray solution, IloNumArray wl, IloNumArray wu, int scale_factor);

float_t dot_product(IloNumArray x, IloNumArray wu);

void compute_R(IloNumArray solution, IloNumArray2 x, IloNumArray y, IloNumArray r);

void debug(int i);

void print_conflicts(IloEnv env, IloModel model, IloCplex cplex);

std::ifstream input(int argc, char **argv, bool& generated_instances, int& cardinality, 
		   int& dimensionality, int& scale_factor, std::string& instance, IloInt& d_0, IloInt& k_0,
		   std::string& path, std::string& filename);
		   
std::ifstream read_solutions(int& solution_n, int& percentage, std::string& sol, bool generated_instances, 
					std::string filename, std::string path, int k_0, int d_0, IloNumArray2 x, std::string instance, int dimensionality);

std::string save_results(std::fstream& sdest_file, bool generated_instances, int dimensionality, int k, int d, int scale_factor, 
					int time_span, IloCplex cplex, IloNumVar z, int d_0, int percentage, int errors, 
					IloNumVarArray a, IloAlgorithm::Status st, std::string filename);

void mismatching_points(int& errors, IloCplex cplex, int k_0, int d_0, int k, std::string path, std::string instance, int percentage, IloNumVarArray s);

#endif
