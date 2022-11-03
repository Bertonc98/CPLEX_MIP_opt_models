#include "../src/utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ilcplex/ilocplex.h>

void compute_W(IloNumArray solution, IloNumArray wl, IloNumArray wu, int scale_factor){
	int len = solution.getSize()-1;
	//~ Computing bounds
	for( int i = 1; i < len; i++ ){
		//~ Positive case: lower the half, upper the double
		if( solution[i] > 0 ){
			wl[i-1] = (float_t)solution[i]/scale_factor;
			wu[i-1] = (float_t)solution[i]*scale_factor;
		}
		//~ Negative case: lower the double, upper the half
		else if( solution[i] < 0 ){
			wl[i-1] = (float_t)solution[i]*scale_factor;
			wu[i-1] = (float_t)solution[i]/scale_factor;
		}
		else{
			wl[i-1] = -scale_factor;
			wu[i-1] = scale_factor;
		}
	}
	
}

float_t dot_product(IloNumArray x, IloNumArray solution){
	//The intercept is added previously
	float_t res = solution[0];
	
	//~ if( x.getSize() != solution.getSize() )
		//~ std::length_error("Point features must be one less than the solution!");

	//~ Dot product x * solution cohefficients
	for( int i = 0; i < x.getSize(); i++ ){
		res += x[i] * solution[i+1];
	}

	return res;
}

void compute_R(IloNumArray solution, IloNumArray2 x, IloNumArray y, IloNumArray r){
	int solution_size =  x.getSize();
	
	for( int i = 0; i < solution_size; i++ ){
		r[i] = abs( y[i] - dot_product(x[i], solution) );		
	}
}

void debug(int i){
	std::cout << "HERE: " << i << std::endl << std::endl;
}
