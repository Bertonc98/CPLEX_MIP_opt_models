#include "../src/utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ilcplex/ilocplex.h>
#include <chrono>

ILOSTLBEGIN

using namespace std;

int main(int argc, char **argv){
	
	string m = argv[1];
	string i = argv[2];
	string d = argv[3];
	
	IloEnv env;
	IloModel model(env);
	IloCplex cplex(env);
	IloNumVarArray vars(env);
	IloObjective obj;
	IloRangeArray rngs(env);
	IloSOS1Array sos1;
	IloSOS2Array sos2;
	IloRangeArray lazy; 
	IloRangeArray cuts;
	
	string path = "../src/data/" + m +"s/generated_results/";
	string instance =  path + m + i + "_50_" + d + ".lp";
	
	cout << instance <<endl;
	//cplex.importModel(model, instance.c_str(), obj, vars, rngs, sos1, sos2, lazy, cuts);
	//~ cplex.importModel(model, "../src/data/node_aggressive0.lp", obj, vars, rngs, sos1, sos2, lazy, cuts);
	cplex.extract(model);	
	
	int n = cplex.getNcols();
	for(int i = 0; i<n; i++){
		model.add(IloConversion(env, vars[i], ILOFLOAT));
	}
	
	
	
	//cplex.setParam(IloCplex::Param::TimeLimit, 5);
	cplex.solve();
	
	
	//cout << "RESULTS:" << endl;
	
	//~ string res_file = path + "hyperplane_" + m + "_" + i + "_" + d + ".dat";
	//~ string res_file = path + "uncut_hyperplane_" + m + "_" + i + "_" + d + ".dat";
	string res_file = path + "aggressive_hyperplane_" + m + "_" + i + "_" + d + ".dat";
	
	// Save features
	//~ string f_file = path + "feature_" + m + "_" + i + "_" + d + ".dat";
	//~ string f_file = path + "uncut_feature_" + m + "_" + i + "_" + d + ".dat";
	string f_file = path + "aggressive_feature_" + m + "_" + i + "_" + d + ".dat";
	
	// Save outliers
	//~ string o_file = path + "outlier_" + m + "_" + i + "_" + d + ".dat";
	//~ string o_file = path + "uncut_outlier_" + m + "_" + i + "_" + d + ".dat";
	string o_file = path + "aggressive_outlier_" + m + "_" + i + "_" + d + ".dat";
	
	
	ofstream mf(res_file);
	ofstream ff(f_file);
	ofstream of(o_file);
	
	string name;
	for(int i = 0; i<n; i++){
		name = string(vars[i].getName(), 0, 1);
		if (name.compare("a") == 0 || name.compare("z") == 0 || name.compare("w") == 0){
			cout << vars[i].getName() << " : " << cplex.getValue(vars[i]) << endl;
			mf << name + "~" + to_string(cplex.getValue(vars[i])) + ";";
		}
		if (name.compare("f") == 0){
			cout << vars[i].getName() << " : " << cplex.getValue(vars[i]) << endl;
			ff << name + "~" + to_string(cplex.getValue(vars[i])) + ";";
		}
		if (name.compare("s") == 0){
			cout << vars[i].getName() << " : " << cplex.getValue(vars[i]) << endl;
			of << name + "~" + to_string(cplex.getValue(vars[i])) + ";";
		}
		
	}
		
	cout << "Hyperplane saved" << endl;
	return 0;
}


