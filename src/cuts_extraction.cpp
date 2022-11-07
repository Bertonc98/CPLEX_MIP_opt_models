#include "../src/utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

// getLBs
// getUBs


ILOBRANCHCALLBACK0(BrCallback) {
		//~ Salvataggio lp del nodo
		//~ CPXLPptr  nodelp;
		//~ IloEnv env = getEnv();
		//~ CPXENVptr cpxenv; 
		//~ cpxenv = (CPXENVptr) &(env);
		//~ void       *cbdata;
		
		//~ CPXgetcallbacknodelp (cpxenv, cbdata, CPX_CALLBACK_MIP_BRANCH, &nodelp);
	 
		//~ CPXwriteprob (cpxenv, nodelp, "cuts.lp", NULL);
		
		IloModel nodelp = getModel();
		
		cout << "CULO" << endl;
		
		IloCplex cplex(getEnv());
		
		cplex.extract(nodelp);
		
		cplex.exportModel("cuts.lp");
		
		
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



