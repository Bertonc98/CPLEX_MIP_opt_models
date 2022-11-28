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
	IloNumArray values(env, 64);
	
	// Generation of the initial model
	if( argc == 1 ){
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

		//~ std::cout.setstate(std::ios::failbit);

		cplex.solve();

		//~ std::cout.clear();
		
		//~ IloNumVarArray cb = cplex.getVariables();
		
		
		
		cplex.getValues(cb, values); 
	
		cout << values << endl;
		
		cout << cplex.getObjValue() <<endl;
	}
	// Resolution of the updated model
	else if( argc == 2 ){
		IloNumVarArray cb(env, 64);
		IloObjective obj;
		IloRangeArray rng;
		
		string path = "../src/data/";
		string lp_name = argv[1];
		path.append(lp_name);
		
		
		IloCplex cplex(env);
		cplex.importModel(model, path.data(), obj, cb, rng);
		cout << "Model imported: "<< path <<endl;
		
		cplex.extract(model);
		cplex.solve();
		
		cplex.getValues(cb, values); 
	
		cout << "Decision variables assignment: " <<endl << values << endl;
		
		cout << "Objective value: " << cplex.getObjValue() <<endl;
	}
	else{
		cout << "USAGE" <<endl;
		cout << "./tower *model_file" <<endl;
		cout << "The model file is optional, if not provided it solves and created the basic tower problem lp" <<endl;
		cout << "and is is saved in src/data as tower_model.lp" << endl;
		cout << "If the model_file is provided and it is in src/data the new model is solved and in output" << endl;
		cout << "are put the variables assignment and the objective value" <<endl;
	}
	
	

	env.end();


	return 0;
}
