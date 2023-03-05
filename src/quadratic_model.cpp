#include "../src/utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>	
#include <cstdlib>
#include <ilcplex/ilocplex.h>
#include <chrono>

ILOSTLBEGIN

int main(int argc, char **argv){
	IloEnv env;
	
	bool generated_instances = false;
	int cardinality, dimensionality, scale_factor;
	
	string instance;
	IloInt d_0;
	IloInt k_0;
	
	string path;
	string filename;
	
	ifstream ifile = input(argc, argv, generated_instances, cardinality, dimensionality, scale_factor, instance, d_0, k_0, path, filename);
	
	
	//~ Read instances
	IloNumArray2 x(env);
	IloNumArray y(env);
	ifile >> x >> y;
	
	//~ Read solutions
	int solution_n = 10;
	int percentage;
	string sol;
	
	ifstream sfile = read_solutions(solution_n, percentage, sol, generated_instances, filename, path, k_0, d_0, x, instance, dimensionality);
	

	//~ Read solutions
	IloNumArray solution(env, solution_n);
	//~ Parsing from file
	for(int i=0; i<solution_n; i++)
		sfile >> solution[i];
	
	int d = solution.getSize()-1;
	IloNumArray wl(env, d);
	IloNumArray wu(env, d);
	
	compute_W(solution, wl, wu, 10);
	
	int k = x.getSize();
	IloNumArray r(env, k, -IloInfinity, IloInfinity);
	
	compute_R(solution, x, y, r);
	if(generated_instances){
		for(int i = 0; i<k; i++){
			r[i] *= 2;
		} 
	}
		
	//~ END OF PREPROCESSING
	
	//~ MODEL DEFINITION
	IloModel model(env);
	
	//~ Variable Definition
	//Slopes
	IloNumVarArray w(env, d, -IloInfinity, IloInfinity); 
	w.setNames("w");
	//Violation cost
	//IloNumVar C(env, 100);
	int C = 10;
	//Measurement error on point i
	IloNumVarArray pp(env, k, 0, IloInfinity); //(7)
	pp.setNames("pp");
	IloNumVarArray pm(env, k, 0, IloInfinity); //(7)
	pm.setNames("pm");
	//Confidence region parameter
	//IloNumVar eps(env, 0);
	int eps = 0.5;
	//Intercept
	IloNumVar z(env, -IloInfinity, IloInfinity); 
	z.setName("z");
	//binary selection of features
	IloNumVarArray f(env, d, 0, 1, ILOINT); // (8)
	f.setNames("f");
	
	//~ Objective Function 
	//~ IloObjective obj = IloAdd(model, IloMinimize(env, IloSum(p - IloScalProd( IloScalProd(mones, s) + ones, r) ) ) );
	int db = 0;
	
	IloObjective obj(env);
	//~ obj.setSense(IloObjective::Minimize); (1)
	IloExpr obj_expr(env);
	
	for( int j = 0; j < d ; j++){
		obj_expr += (0.5)*(w[j]*w[j]);
	}
	for( int i = 0; i < k ; i++){
		obj_expr += C*(pp[i] + pm[i]);
	}
	
	obj.setExpr(obj_expr);
	obj_expr.end();
	
	IloAdd(model, IloMinimize(env, obj));
	
	//~ Constraints over k (the cardinality of the points)
	
	for( int i = 0; i < k; i++){
		//~ Referring to the pdf description of the problem
		model.add( (IloScalProd(w, x[i]) + z - y[i]) <= (eps + pp[i]) ); // (2)
		model.add( (-IloScalProd(w, x[i]) - z + y[i]) <= (eps + pm[i]) ); // (3)
	}
	
	//~ Constraint over d (number of features)
	for( int j = 0; j < d; j++){
		//~ Referring to the pdf description of the problem
		model.add( w[j] <= (f[j]*wu[j]) ); // (4)
		model.add( w[j] >= (f[j]*wl[j]) ); // (5)
	}
	
	//Limit the nimber of points to k_0
	model.add(IloSum(f) <= k_0); // (6)
	
	
	IloCplex cplex(env);
	//~ cplex.add(obj_expr);
	
	cplex.extract(model);
	
	string export_file;
	
	if(generated_instances){
		export_file = "../src/data/quadratic_models/generated_results/quadratic_model"+ instance + "_" + to_string(cardinality) + ".lp"; 
	}
	else{
		export_file = "../src/data/quadratic_models/quadratic_model"+ instance + "_" + to_string(d_0) + "_" + to_string(k_0) + ".lp"; 
	}
	
	cplex.exportModel(export_file.c_str());
	cout <<"EPSILON" << eps << endl;
	
	//Suppress the outpt
	//std::cout.setstate(std::ios::failbit);
	// Resolution time
	cplex.setParam(IloCplex::Param::TimeLimit, 500);
	chrono::steady_clock sc;  
	auto start = sc.now();     // start timer

	cplex.solve();

	auto end = sc.now();       // end timer 
	auto time_span = static_cast<chrono::duration<double>>(end - start).count();   // measure time span between start & end
	
	//std::cout.clear();
	
	IloAlgorithm::Status st = cplex.getStatus();
	cout <<"Status: " <<  st <<endl;
	if(st != 2)
		print_conflicts(env, model, cplex);
	
	int errors = 0;
	
	//~ Saving results
	
	string res_name;
	
	if(generated_instances){
		res_name = "../src/data/SFSOD/generated_results/quadratic_results.csv";
	}
	else{
		res_name = "../src/data/SFSOD/quadratic_results.csv";
	}
	fstream dest_file;
	string line = "";
	
	
	ifstream myfile;
	myfile.open(res_name);
	if(!myfile) {
		//cout<<"file not exists"<<endl;
		if(generated_instances){
			line = "Instance;d;k;w_bound;Time;OurObj;intercept;slopes\n";
		}
		else{
			line = "Instance;d_0;k_0;OurObj;intercept;slopes\n";
		}
	} 
	
	dest_file.open(res_name, fstream::app);
	
	if(generated_instances){
		if(st!=2){
			line += filename + ";" + to_string(dimensionality) + ";" + to_string(k) + ";" + to_string(scale_factor) + ";" + to_string(time_span) + ";None;None;";
		}
		else{
			line += filename + ";" + to_string(dimensionality) + ";" + to_string(k) + ";" + to_string(scale_factor) + ";" + to_string(time_span) + ";" + to_string(cplex.getObjValue()) + ";" + to_string(cplex.getValue(z)) + ";";
		}
		
	}
	else{
		line += filename + ";" + to_string(d_0) + ";" + to_string(percentage) + ";" + to_string(cplex.getObjValue()) + ";" + to_string(cplex.getValue(z)) + ";";
	}
	for(int i=0; i<d; i++){
		if(st != 2){
			if(i != d-1)
				line += "None~";
			else
				line += "None";
		}
		else{
			if(i != d-1)
				line += to_string(cplex.getValue(w[i])) + "~";
			else
				line += to_string(cplex.getValue(w[i]));
		}
	}
	
	dest_file<<line<<endl;
	
	
	cout << "Obj value: " << cplex.getObjValue() << endl;
	
	env.end();
	cout << "Environment destroyed." << endl;

	return 0;
}

