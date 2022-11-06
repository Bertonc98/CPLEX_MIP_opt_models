#include "../src/utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

ILOBRANCHCALLBACK0(BrCallback) {
		//~ Salvataggio lp del nodo
		IloEnv env;
		
		IloModel nodelp = getModel();
		
		IloCplex cplex(env);
	
		cplex.extract(nodelp);
		cplex.exportModel("cuts.lp");
		
		cout << "CULO" << endl;
		prune();
	}

int main(int argc, char **argv){
  
	int 		status = 0;
	IloEnv 		env;
	IloModel    model(env);
	IloCplex 	cplex(env);
	const char*	filename = argv[1];
	
	cplex.importModel(model, filename);
	
	cplex.use(BrCallback(env));
	cplex.extract(model);
	
	cplex.solve();
	
	env.end();
	
	return 0;
}



