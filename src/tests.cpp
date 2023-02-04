#include "../src/utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>	
#include <cstdlib>
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

/*
 * From "Modelli MPHero the Disjunctive Linearization of the Projected McCormick's model
 * 
 */

int main(int argc, char **argv){
	cout << "Creating envirnment..." << endl;
	IloEnv env;
	
	IloNumArray2 x(env);
	
	x.add(IloNumArray(env, 3));
	x.add(IloNumArray(env, 3));
	
	for(int i =0; i<2; i++){
		for(int j=0; j<3; j++){
			x[i][j] = 7;
		}
	}
	
	cout << x <<endl;
	
	ifstream infile("../src/data/node_aggressive0.lp");
	
	string line;
	while (std::getline(infile, line))
	{
    istringstream iss(line);
    cout << line << "---" << endl;

	}
	

	return 0;
}


