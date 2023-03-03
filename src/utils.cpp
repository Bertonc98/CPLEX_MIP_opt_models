#include "../src/utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ilcplex/ilocplex.h>

void compute_W(IloNumArray solution, IloNumArray wl, IloNumArray wu, int scale_factor){
	int len = solution.getSize();
	//~ Computing bounds
	for( int i = 1; i < len; i++ ){
		//~ Positive case: lower the half, upper the double
		if( solution[i] > 0 ){
			wu[i-1] = (float_t)solution[i]*scale_factor;
			wl[i-1] = (float_t)solution[i]-wu[i-1];
		}
		//~ Negative case: lower the double, upper the half
		else if( solution[i] < 0 ){
			wl[i-1] = (float_t)solution[i]*scale_factor;
			wu[i-1] = (float_t)solution[i]-wl[i-1];
		}
		else{
			wl[i-1] = -scale_factor;
			wu[i-1] = scale_factor;
		}
	}
	
}

void compute_W_optimal_hyperplane(IloNumArray solution, IloNumArray wl, IloNumArray wu, int scale_factor){
	//~ The optimal hyperplane is composed by the first 5 features set to 1, the others to 0
	//~ No bound for the last coefficient, the intercept
	std::cout << "Scale factor: " << scale_factor <<std::endl;
	int len = solution.getSize();
	//~ Computing bounds
	for( int i = 1; i < len; i++ ){
		wl[i-1] = ((float_t)solution[i] + 2) * scale_factor;
		wu[i-1] = ((float_t)solution[i] - 2) * scale_factor;
	}
	
}

float_t dot_product(IloNumArray x, IloNumArray solution){
	//The intercept is added previously
	float_t res = solution[0];
	
	//~ Dot product x * solution cohefficients
	for( int i = 0; i < x.getSize(); i++ ){
		res += x[i] * solution[i+1];
	}

	return res;
}

void compute_R(IloNumArray solution, IloNumArray2 x, IloNumArray y, IloNumArray r){
	int solution_size =  x.getSize();
	float max = 0;
	float error;
	
	for( int i = 0; i < solution_size; i++ ){		
		error = abs( y[i] - dot_product(x[i], solution) );		
		if(error > max)
			max = error;
	}
	
	for( int i = 0; i < solution_size; i++ ){
		r[i] = max;
	}
}

void debug(int i){
	std::cout << "HERE: " << i << std::endl << std::endl;
}

void print_conflicts(IloEnv env, IloModel model, IloCplex cplex){
	IloConstraintArray constraints(env);
	for (IloModel::Iterator it(model); it.ok(); ++it) {
         IloExtractable ext = *it;
         if (ext.isVariable()) {
            IloNumVar v = ext.asVariable();
            // Add variable bounds to the constraints array.
            constraints.add(IloBound(v, IloBound::Lower));
            constraints.add(IloBound(v, IloBound::Upper));
         }
         else if (ext.isConstraint()) {
            IloConstraint c = ext.asConstraint();
            constraints.add(c);
         }
      }
      
      IloNumArray prefs(env, constraints.getSize());
      for (int i = 0; i < prefs.getSize(); ++i)
         prefs[i] = 1.0;
	if (cplex.refineConflict(constraints, prefs)) {
		 // Display the solution status.
		 IloCplex::CplexStatus status = cplex.getCplexStatus();
		 std::cout << "Solution status = " << status << " (" <<
			static_cast<int>(status) << ")" << std::endl;

		 // Get the conflict status for the constraints that were specified.
		 IloCplex::ConflictStatusArray conflict = cplex.getConflict(constraints);

		 // Print constraints that participate in the conflict.
		 std::cout << "Conflict:" << std::endl;

		 for (int i = 0; i < constraints.getSize(); ++i) {
			if (conflict[i] == IloCplex::ConflictMember ||
				conflict[i] == IloCplex::ConflictPossibleMember) {
			   std::cout << "  " << constraints[i] << std::endl;
			}
		 }
	 }
}
