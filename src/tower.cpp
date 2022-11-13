#include "../src/utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ilcplex/ilocplex.h>


ILOSTLBEGIN

using namespace std;

int main(int argc, char **argv){

	IloEnv env;

	//~ MODEL DEFINITION
	IloModel model(env);

	//~ Variable Definition
	IloNumVarArray cb(env, 64, 0, 1, ILOINT);
	cb.setNames("cb");

	//~ Objective Function
	IloObjective obj = IloAdd(model, IloMaximize(env, IloSum(cb)));

	//~ int db = 0;




	for( int i = 0; i < 8 ; i++){
    IloExpr expr(env);
    for( int j = 0; j < 8 ; j++){
      expr += cb[(i*8) + j];
    }
    model.add(IloRange(expr <= 1));
  	expr.end();
	}


	IloCplex cplex(env);
	cplex.extract(model);

	cplex.exportModel("../src/data/tower_model.lp");

	std::cout.setstate(std::ios::failbit);

	cplex.solve();

	std::cout.clear();
	
	IloNumArray values(env, 64);
	cplex.getValues(cb, values); 
	
	cout << values << endl;

	env.end();


	return 0;
}
