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

#endif
