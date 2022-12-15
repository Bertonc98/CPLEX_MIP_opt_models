#include "../src/utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>	
#include <cstdlib>
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

int main(int argc, char **argv){
	cout << "Creating envirnment..." << endl;
	IloEnv env;
	if(argc != 4){
		cout << "Wrong number of parameters"<<endl;
		cout << "./basic_model d k instance_number"<<endl;
		return 1;
	}
	
	string instance = argv[1];
	IloInt d_0 = stoi(argv[2]);
	IloInt k_0 = stoi(argv[3]);
	

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
	
	int d = solution.getSize()-1;
	IloNumArray wl(env, d);
	IloNumArray wu(env, d);
	
	compute_W(solution, wl, wu, 10);
	
	int k = x.getSize();
	IloNumArray r(env, k, -IloInfinity, IloInfinity);
	
	compute_R(solution, x, y, r);
	
	
	
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
	pp.setNames("p+");
	IloNumVarArray pm(env, k, 0, IloInfinity); //(7)
	pm.setNames("p-");
	//Confidence region parameter
	//IloNumVar eps(env, 0);
	int eps = 1;
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
		obj_expr += (1/2)*(w[j]*w[j]);
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
	
	string export_file = "../src/data/quadratic_models/quadratic_model"+ instance + "_" + to_string(d_0) + "_" + to_string(k_0) + ".lp"; 
	cplex.exportModel(export_file.c_str());
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
		cout << "./basic_model instance_number d k"<<endl;
		return 1;
	}
	
	/*
	//~ Mismatching count 
	int errors = 0;
	//~ cout << "Pnt, Out | Out model " << endl;
	string tmp;
	int result, pos;
	for (int i = 0; i < k ; i++){
		getline(cfile, tmp);
		result = 1-int(abs(cplex.getValue(f[i])));
		pos = tmp.find(",");
		if( result != stoi(tmp.substr(pos+1, 1)) ){
			//~ cout << tmp << " | " << 1-int(abs(cplex.getValue(s[i]))) << endl;
			errors++;
		}
	}
	
	//~ Saving results
	fstream dest_file;
	string res_name = "../src/data/quadratic_results.csv";
	dest_file.open(res_name, fstream::app);
	if(dest_file.fail()){
	//~ cout<<"Destination file not exists: creating..."<<endl;
		ofstream create_file(res_name);
		create_file<<"";
		create_file.close();
		dest_file.open(res_name, fstream::app);
		dest_file << "Instance;d_0;k_0;MismatchedOutliers;OurObj;YourObj"<<endl;
	}
	
	string line = filename + ";" + to_string(d_0) + ";" + to_string(percentage) + ";" + to_string(errors) + ";" + to_string(cplex.getObjValue());

	dest_file<<line<<endl;
	
	*/
	cout << "Obj value: " << cplex.getObjValue() << endl;
	
	env.end();
	cout << "Environment destroyed." << endl;

	return 0;
}

