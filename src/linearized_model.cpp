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
	if(argc != 5){
		cout << "Wrong number of parameters"<<endl;
		cout << "./linearized_model d k instance_number outlier_number"<<endl;
		return 1;
	}
	
	string instance = argv[1];
	IloInt d_0 = stoi(argv[2]);
	IloInt k_0 = stoi(argv[3]);
	IloInt s_0 = stoi(argv[4]);
	

	string path = "../src/instance_set/";
	string filename  = path + "toy_30_10_02_2_0_5_-10_" + instance + ".dat";
	ifstream ifile(filename);
	if (!ifile) {
		cerr << "ERROR: could not open instance file '" << filename << endl;
		cout << "./basic_model d k instance_number"<<endl;	
		return 1;
	}
	
	//~ Read instances
	IloNumArray2 x(env);
	IloNumArray y(env);
	ifile >> x >> y;
	
	int percentage = ((float_t)k_0 / (float_t)x.getSize())*10;
		
	string sol = path + "optimal_solutions/minError_toy_30_10_02_2_0_5_-10_" + instance + 
				 "_l1_LinMgr_indicator_L0Mgr_sos1_" + to_string(d_0) +
				 ".000000_0." + to_string(percentage) + 
			     "00000Result.dat";

	
	ifstream sfile(sol);
	if (!sfile) {
		cerr << "ERROR: could not open solution file '" << sol << endl;
		cout << "./basic_model instance_number d k"<<endl;
		return 1;
	}
	
	cout << "Working with: " << sol << endl;

	//~ Read solutions
	int solution_n = 10;
	IloNumArray solution(env, solution_n);
	//~ Parsing from file
	for(int i=0; i<solution_n; i++)
		sfile >> solution[i];
	
	// d = J, the feature/dimension number
	int d = solution.getSize()-1;
	IloNumArray wl(env, d);
	IloNumArray wu(env, d);
	
	compute_W(solution, wl, wu, 10);
	
	//K = I, amount of instances
	int k = x.getSize();
	IloNumArray Rp(env, k, -IloInfinity, IloInfinity);
	IloNumArray Rm(env, k, -IloInfinity, IloInfinity);
	
	compute_R(solution, x, y, Rp);
	compute_R(solution, x, y, Rm);
	
	for(int i = 0; i<k; i++){
		Rp[i] *= 1000000;
		Rm[i] *= 1000000;
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
	IloNumVarArray pp(env, k, 0, IloInfinity); //(45)
	pp.setNames("pp");
	IloNumVarArray pm(env, k, 0, IloInfinity); //(45)
	pm.setNames("pm");
	//Confidence region parameter
	//IloNumVar eps(env, 0);
	int eps = 0.2;
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
		obj_expr += C*(pp[i] - (1-s[i])*Rp[i]);
	}
	for( int i = 0; i < k ; i++){
		obj_expr += C*(pm[i] - (1-s[i])*Rm[i]);
	}
	
	obj.setExpr(obj_expr);
	obj_expr.end();
	
	IloAdd(model, IloMinimize(env, obj));
	
	//~ Constraints over k (I) (the cardinality of the points)
	
	for( int i = 0; i < k; i++){
		//~ Referring to the pdf description of the problem
		model.add( (IloScalProd(w, x[i]) + z - y[i]) <= (eps + pp[i]) ); // (37)
		model.add( (-IloScalProd(w, x[i]) - z + y[i]) <= (eps + pm[i]) );// (38)
		model.add( (Rp[i]*(1-s[i])) <= pp[i] );				 // (39)
		model.add( pp[i] <= Rp[i] );	 				 // (39)
		model.add( (Rm[i]*(1-s[i])) <= pm[i] );				 // (40)
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
	model.add(IloSum(s) >= s_0); // (44)
		
	IloCplex cplex(env);
	//~ cplex.add(obj_expr);
	
	cplex.extract(model);
	
	string export_file = "../src/data/linearized_models/linearized_model"+ instance + "_" + to_string(d_0) + "_" + to_string(k_0) + ".lp"; 
	cplex.exportModel(export_file.c_str());
	
	// Save d and k values
	ofstream fw("../src/data/feature_point.txt", ofstream::out);
	fw << d << "\n";
	fw << k << "\n";
	
	cout <<"EPSILON" << eps << endl;
	
	//Suppress the outpt
	//std::cout.setstate(std::ios::failbit);
 
	cplex.solve();
	
	//std::cout.clear();
	
	IloAlgorithm::Status st = cplex.getStatus();
	cout <<"Status: " <<  st <<endl;
	
	
	for( int i = 0; i < 50 ; i++) cout << "=";
	cout << endl << "k_0 = " << k_0 << endl;
	
	string compare = path + "optimal_solutions/minError_toy_30_10_02_2_0_5_-10_" + instance + 
				 "_l1_LinMgr_indicator_L0Mgr_sos1_" + to_string(d_0) +
				 ".000000_0." + to_string(percentage) + 
			     "00000Outlier.csv";
	ifstream cfile;
	cfile.open(compare);
	if (!cfile) {
		cerr << "ERROR: could not open comparison file '" << compare << endl;
		cout << "./linearized_model instance_number d k s"<<endl;
		return 1;
	}
	/* CONFLICTS */
	//If not feasible
	if(st != 2)
		print_conflicts(env, model, cplex);

	// */
	
	
	//~ Mismatching count 
	int errors = 0;
	cout << "Pnt, Out | Out model " << endl;
	string tmp;
	int result, pos;
	for (int i = 0; i < k ; i++){
		getline(cfile, tmp);
		result = int(abs( cplex.getValue(s[i]) ));
		pos = tmp.find(",");
		if( result != stoi(tmp.substr(pos+1, 1)) ){
			cout << tmp << " | " << result << endl;
			errors++;
		}
	}
	cout << "errors: " << errors << endl;
	
	//Output s and f values
	cout << endl << "s: \t";
	for(int i =0; i<k; i++){
		cout << cplex.getValue(s[i]) << " ";
	}
	
	cout << endl << "f: \t";
	for(int i =0; i<d; i++){
		cout << cplex.getValue(f[i]) << " ";
	}
	cout << endl;
	
	//~ Saving results
	fstream dest_file;
	string res_name = "../src/data/SFSOD/linearized_results.csv";
	string line = "";
	
	ifstream myfile;
	myfile.open(res_name);
	if(!myfile) {
		cout<<"file not exists"<<endl;
		line = "Instance;d_0;k_0;MismatchedOutliers;OurObj;intercept;slopes\n";
	} 
	
	dest_file.open(res_name, fstream::app);
	
	line += filename + ";" + to_string(d_0) + ";" + to_string(percentage) + ";" + to_string(errors) + ";" + to_string(cplex.getObjValue()) + ";" + to_string(cplex.getValue(z)) + ";";
	
	for(int i=0; i<d; i++){
		if(i != d-1)
			line += to_string(cplex.getValue(w[i])) + "~";
		else
			line += to_string(cplex.getValue(w[i]));
	}
	
	dest_file<<line<<endl;
	
	
	cout << "Obj value: " << cplex.getObjValue() << endl;
	env.end();
	cout << "Environment destroyed." << endl;

	return 0;
}


