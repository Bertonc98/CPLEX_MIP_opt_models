#include "../src/utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>	
#include <cstdlib>
#include <ilcplex/ilocplex.h>
#include <chrono>

ILOSTLBEGIN

/*
 * From "Modelli MPHero the Disjunctive Linearization of the Projected McCormick's model
 * 
 */

int main(int argc, char **argv){
	
	IloEnv env;
	
	bool generated_instances = false;
	int cardinality, dimensionality;
	double_t scale_factor = -1.0;
	
	string instance;
	IloInt d_0;
	IloInt k_0;
	IloInt s_0;
	
	string path;
	string filename;
	
	ifstream ifile = input(argc, argv, generated_instances, cardinality, dimensionality, scale_factor, instance, k_0, s_0, path, filename);
	
	d_0 = s_0;
		
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
	
	// d = J, the feature/dimension number
	int d = solution.getSize()-1;
	IloNumArray wl(env, d);
	IloNumArray wu(env, d);
	
	if(generated_instances){
		compute_W_optimal_hyperplane(solution, wl, wu, scale_factor);
	}
	else{
		compute_W(solution, wl, wu, 10.0);
	}
	
	//K = I, amount of instances
	int k = x.getSize();
	IloNumArray Rp(env, k, -IloInfinity, IloInfinity);
	IloNumArray Rm(env, k, -IloInfinity, IloInfinity);
	
	if(generated_instances){
		compute_RpRm(solution, x, y, Rp, Rm, 5.0);
	}
	else{
		compute_RpRm(solution, x, y, Rp, Rm, 1.0);
	}
	
	
	
	/*for(int i = 0; i<k; i++){
		Rp[i] *= 1000000;
		Rm[i] *= 1000000;
	} */
	
	//~ END OF PREPROCESSING
	
	//~ MODEL DEFINITION
	IloModel model(env);
	
	//~ Variable Definition
	//Slopes
	IloNumVarArray w(env, d, -IloInfinity, IloInfinity); 
	w.setNames("w");
	//Violation cost
	//IloNumVar C(env, 100);
	double_t C = 10.0;
	//Measurement error on point i
	IloNumVarArray pp(env, k, 0, IloInfinity); //(45)
	pp.setNames("pp");
	IloNumVarArray pm(env, k, 0, IloInfinity); //(45)
	pm.setNames("pm");
	//Confidence region parameter
	//IloNumVar eps(env, 0);
	double_t eps = 0.2;
	//Intercept
	IloNumVar z(env, -IloInfinity, IloInfinity); 
	z.setName("z");
	//binary selection of features
	IloNumVarArray f(env, d, 0, 1, ILOINT); // (46)
	f.setNames("f");
	//outlier detection
	IloNumVarArray s(env, k, 0, 1, ILOINT); // (47)
	s.setNames("s");
	
	//Setting of R
	
	//~ Objective Function 
	//~ IloObjective obj = IloAdd(model, IloMinimize(env, IloSum(p - IloScalProd( IloScalProd(mones, s) + ones, r) ) ) );
	int db = 0;
	
	IloObjective obj(env);
	// objective function //(36)
	IloExpr obj_expr(env);
	
	for( int j = 0; j < d ; j++){
		obj_expr += (0.5)*(w[j]*w[j]);
	}
	for( int i = 0; i < k ; i++){
		obj_expr += C*(pp[i] - (1.0-s[i])*Rp[i]);
	}
	for( int i = 0; i < k ; i++){
		obj_expr += C*(pm[i] - (1.0-s[i])*Rm[i]);
	}
	
	obj.setExpr(obj_expr);
	obj_expr.end();
	
	IloAdd(model, IloMinimize(env, obj));
	
	//~ Constraints over k (I) (the cardinality of the points)
	
	for( int i = 0; i < k; i++){
		//~ Referring to the pdf description of the problem
		model.add( (IloScalProd(w, x[i]) + z - y[i]) <= (eps + pp[i]) ); // (37)
		model.add( (-IloScalProd(w, x[i]) - z + y[i]) <= (eps + pm[i]) );// (38)
		model.add( (Rp[i]*(1.0-s[i])) <= pp[i] );				 // (39)
		model.add( pp[i] <= Rp[i] );	 				 // (39)
		model.add( (Rm[i]*(1.0-s[i])) <= pm[i] );				 // (40)
		model.add( pm[i] <= Rm[i] );	 				 // (40)
	}
	
	//~ Constraint over d (J) (number of features)
	for( int j = 0; j < d; j++){
		//~ Referring to the pdf description of the problem
		model.add( w[j] <= (f[j]*wu[j]) ); // (41)
		model.add( w[j] >= (f[j]*wl[j]) ); // (42)
		
	}
	
	//Limit the number of points to k_0
	model.add(IloSum(f) <= k_0); // (43)
	//Ensure a certain amount of selected outliers
	model.add(IloSum(s) >= k - s_0); // (44)
		
	IloCplex cplex(env);
	//~ cplex.add(obj_expr);
	
	cplex.extract(model);
	string export_file;
	
	if(generated_instances){
		export_file = "../src/data/linearized_models/generated_results/linearized_model"+ instance + "_" + to_string(cardinality) + ".lp"; 
	}
	else{
		export_file = "../src/data/linearized_models/linearized_model"+ instance + "_" + to_string(d_0) + "_" + to_string(k_0) + ".lp"; 
	}
	
	cplex.exportModel(export_file.c_str());
	exit(0);
	// Save d and k values
	ofstream fw("../src/data/feature_point.txt", ofstream::out);
	fw << d << "\n";
	fw << k << "\n";
	
	cout <<"EPSILON" << eps << endl;
	
	//Suppress the outpt
	//std::cout.setstate(std::ios::failbit);
	// Resolution time
	cplex.setParam(IloCplex::Param::TimeLimit, 300);
	//cplex.setParam(IloCplex::Param::MIP::Limits::Nodes, 1);
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
	string model_name = argv[0];	
	string md = model_name.substr(2, model_name.find("_"));

	//mismatching_points(errors, cplex, k_0, d_0, k, path, instance, percentage, s, f, md, generated_instances, d);
	
	//Output s and f values
	cout << endl << "s: \t";
	for(int i =0; i<k; i++){
		cout << cplex.getValue(s[i]) << " ";
	}
	cout << "limit: >" << k - s_0 << endl;
	
	cout << endl << "f: \t";
	for(int i =0; i<d; i++){
		cout << cplex.getValue(f[i]) << " ";
	}
	cout << "limit: < " << k_0 << endl;
	cout << endl;
	
	//~ Saving results
	fstream dest_file;
	model_name = model_name.substr(2, model_name.find("_"));
	
	string line = save_results(dest_file, generated_instances, dimensionality, k, d, scale_factor, time_span, cplex, z, d_0, percentage, errors, w, st, filename, model_name);	
	dest_file<<line<<endl;
	
	
	cout << "Obj value: " << cplex.getObjValue() << endl;
	env.end();
	cout << "Environment destroyed." << endl;

	return 0;
}


