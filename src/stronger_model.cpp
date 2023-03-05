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
	
	// SCALE FACTOR 10
	if(generated_instances){
		compute_W_optimal_hyperplane(solution, wl, wu, scale_factor);
	}
	else{
		compute_W(solution, wl, wu, 100);
	}
	
	int k = x.getSize();
	IloNumArray r(env, k, -IloInfinity, IloInfinity);
	compute_R(solution, x, y, r);
		
	//~ END OF PREPROCESSING
	
	//~ MODEL DEFINITION
	IloModel model(env);
	
	//~ Variable Definition
	IloNumVarArray a(env, d, -IloInfinity, IloInfinity); // (25)
	a.setNames("a");
	IloNumVar z(env, -IloInfinity, IloInfinity); // (26)
	z.setName("z");
	IloNumVarArray f(env, d, 0, 1, ILOINT); // (27)
	f.setNames("f");
	IloNumVarArray p(env, k, 0.0, IloInfinity); // (28)
	p.setNames("p");
	IloNumVarArray t(env, k, -IloInfinity, IloInfinity); // (29)
	t.setNames("t");
	IloNumVarArray s(env, k, 0, 1, ILOINT); // (30)
	s.setNames("s");
	
	
	//~ Objective Function 
	//~ IloObjective obj = IloAdd(model, IloMinimize(env, IloSum(p - IloScalProd( IloScalProd(mones, s) + ones, r) ) ) );
	int db = 0;
	
	IloObjective obj(env);
	//~ obj.setSense(IloObjective::Minimize);
	IloExpr obj_expr(env);
	
	for( int i = 0; i < k ; i++){
		obj_expr += p[i] - ( (1-s[i])*r[i] );
	}
	obj.setExpr(obj_expr);
	obj_expr.end();
	
	IloAdd(model, IloMinimize(env, obj));
	
	//~ int db = 0;
	
	//~ Constraints over k
	for( int i = 0; i < k; i++){
		//~ Referring to the pdf description of the problem
		model.add(p[i] >= ((1-s[i]) * (r[i]/10) )); // (17)
		model.add(p[i] <= r[i]*10); // (18) Increased bound from r[i]
		
		model.add((p[i] + y[i] - IloScalProd(a, x[i]) - z) >= 0); // (19)
		model.add((-p[i] + y[i] - IloScalProd(a, x[i]) - z) <= 0); // (20)

	}
	
	//~ Constraint over d
	for( int i = 0; i < d; i++){
		//~ Referring to the pdf description of the problem
		model.add(a[i] >= (wl[i] * f[i])); // (21)
		model.add(a[i] <= (wu[i] * f[i])); // (22)
	}
	
	//~ Constraint over upper/lower amount of choosen features
	model.add(IloSum(f) <= d_0); // (23)
	model.add(IloSum(s) >= k - k_0); // (24)
	
	IloCplex cplex(env);
	//~ cplex.add(obj_expr);
	
	cplex.extract(model);
	//cplex.exportModel("../src/data/strong_model.lp");
	//cout << "HERE" << endl;
	
	string export_file;
	
	if(generated_instances){
		export_file = "../src/data/stronger_models/generated_results/stronger_model"+ instance + "_" + to_string(cardinality) + ".lp"; 
	}
	else{
		export_file = "../src/data/stronger_models/stronger_model"+ instance + "_" + to_string(d_0) + "_" + to_string(k_0) + ".lp"; 
	}
	
	cplex.exportModel(export_file.c_str());
	
	
	//Suppress the outpt
	//std::cout.setstate(std::ios::failbit);
	// Resolution time
	cplex.setParam(IloCplex::Param::TimeLimit, 500);
	chrono::steady_clock sc;  
	cout << "========================START SOLVING========================" <<endl;
	auto start = sc.now();     // start timer

	cplex.solve();

	auto end = sc.now();       // end timer 
	auto time_span = chrono::duration_cast<chrono::milliseconds>(end - start).count();   // measure time span between start & end
	cout << "========================END SOLVING========================" <<endl;
	cout << "========================TIME " << time_span << " ========================" <<endl;
	IloAlgorithm::Status st = cplex.getStatus();
	cout <<"Status: " <<  st <<endl;
	if(st != 2)
		print_conflicts(env, model, cplex);
	
	int errors = 0;
	if(!generated_instances){
		mismatching_points(errors, cplex, k_0, d_0, k, path, instance, percentage, s);	
	}
	
	
	//~ Saving results
	fstream dest_file;
	string model_name = argv[0];
	model_name = model_name.substr(0, model_name.find("_"));
	string line = save_results(dest_file, generated_instances, dimensionality, k, d, scale_factor, time_span, cplex, z, d_0, percentage, errors, a, st, filename, model_name);	
	dest_file<<line<<endl;
	
	
	cout << "Obj value: " << cplex.getObjValue() << endl;
	
	env.end();
	cout << "Environment destroyed." << endl;

	return 0;
}

