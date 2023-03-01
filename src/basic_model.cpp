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
	cout << "Creating envirnment..." << endl;
	IloEnv env;
	if(argc < 4){
		cout << "Wrong number of parameters"<<endl;
		cout << "./basic_model instance_number d k cardinality*"<<endl;
		cout << "d is the amount of features that are at most NOT considered"<<endl;
		cout << "k is the number of points that are at least considered as outliers" <<endl;
		cout << "instance_number is the toy_instance considered" <<endl;
		cout << "cardinality is optional. and it will lead to handle the generated instances" <<endl;
		return 1;
	}
	
	bool generated_instances = false;
	int cardinality;
	
	if(argc == 5){
		cardinality = stoi(argv[4]);
		generated_instances = true;
		
	}
	
	string instance = argv[1];
	IloInt d_0 = stoi(argv[2]);
	IloInt k_0 = stoi(argv[3]);
	
	string path;
	string filename;
	if(generated_instances){
		path = "../src/instance_set/generated_instances/";
		filename  = path + "toy_" + to_string(cardinality) + "_10_-" + instance + ".dat";
	}
	else{
		path = "../src/instance_set/";
		filename  = path + "toy_30_10_02_2_0_5_-10_" + instance + ".dat";
	}
	ifstream ifile(filename);
	if (!ifile) {
		cerr << "ERROR: could not open instance file '" << filename << endl;
		cout << "./basic_model instance_number d k cardinality*"<<endl;
		cout << "d is the amount of features that are at most NOT considered"<<endl;
		cout << "k is the number of points that are at least considered as outliers" <<endl;
		cout << "instance_number is the toy_instance considered" <<endl;
		cout << "cardinality is optional. and it will lead to handle the generated instances" <<endl;
		return 1;
	}
	
	//~ Read instances
	IloNumArray2 x(env);
	IloNumArray y(env);
	ifile >> x >> y;
	
	//~ Read solutions
	int solution_n = 10;
	int percentage;
	string sol;
	
	if(!generated_instances){	
		percentage = ((float_t)k_0 / (float_t)x.getSize())*10.0;
			
		sol = path + "optimal_solutions/minError_toy_30_10_02_2_0_5_-10_" + instance + 
					 "_l1_LinMgr_indicator_L0Mgr_sos1_" + to_string(d_0) +
					 ".000000_0." + to_string(percentage) + 
					 "00000Result.dat";
	}
	else{
		sol = path + "hyperplane.dat";
	}
	
	ifstream sfile(sol);
	if (!sfile) {
		cerr << "ERROR: could not open solution file '" << sol << endl;
		cout << "./basic_model instance_number d k"<<endl;
		cout << "d is the amount of features that are at most NOT considered"<<endl;
		cout << "k is the number of points that are at least considered as outliers" <<endl;
		cout << "instance_number is the toy_instance considered" <<endl;
		return 1;
	}
	cout << "Working with: " << sol << endl;
	

	IloNumArray solution(env, solution_n);
	

	for(int i=0; i<solution_n; i++)
		sfile >> solution[i];
	
	int d = solution.getSize()-1;
	IloNumArray wl(env, d);
	IloNumArray wu(env, d);
	
	compute_W(solution, wl, wu, 2);
	
	int k = x.getSize();
	IloNumArray r(env, k);
	
	compute_R(solution, x, y, r);
	
	//~ END OF PREPROCESSING
	
	//~ MODEL DEFINITION
	IloModel model(env);
	
	//~ Variable Definition
	IloNumVarArray q(env, k, 0.0, IloInfinity);
	q.setNames("q");
	IloNumVarArray t(env, k, -IloInfinity, IloInfinity);
	t.setNames("t");
	IloNumVarArray s(env, k, 0, 1, ILOINT);
	s.setNames("s");
	IloNumVar z(env, -IloInfinity, IloInfinity);
	z.setName("z");
	
	IloNumVarArray a(env, d, -IloInfinity, IloInfinity);
	a.setNames("a");
	IloNumVarArray f(env, d, 0, 1, ILOINT);
	f.setNames("f");
	
	//~ Objective Function 
	IloObjective obj = IloAdd(model, IloMinimize(env, IloSum(q)));
	
	//~ int db = 0;
	cout << x[0] <<endl;
	cout<< d <<endl;
	
	//~ Constraints over k
	for( int i = 0; i < k; i++){
		//~ Referring to the pdf description of the problem
		
		//~ model.add((q[i] + y[i] - z - t[i]) >= 0); 
		model.add((q[i] + y[i] - IloScalProd(a, x[i]) - z - t[i]) >= 0); // (2)
		model.add((-q[i] + y[i] - IloScalProd(a, x[i]) - z - t[i]) <= 0); // (3)
		
		model.add(t[i] >= (-(1-s[i]) * (r[i]*2) )); // (4)
		model.add(t[i] <= ((1-s[i]) * (r[i]*2) )); // (5)
		
	}
	
	//~ Constraint over d
	for( int i = 0; i < d; i++){
		//~ Referring to the pdf description of the problem
		model.add(a[i] >= (wl[i] * f[i])); // (6)
		model.add(a[i] <= (wu[i] * f[i])); // (7)
	}
	
	//~ Constraint over upper/lower amount of choosen features
	model.add(IloSum(f) <= d_0);
	model.add(IloSum(s) >= k-k_0);

	
	IloCplex cplex(env);
	cplex.extract(model);
	
	string export_file;
	
	if(generated_instances){
		export_file = "../src/data/basic_models/generated_results/basic_model"+ instance + "_" + to_string(cardinality) + ".lp"; 
	}
	else{
		export_file = "../src/data/basic_models/basic_model"+ instance + "_" + to_string(d_0) + "_" + to_string(k_0) + ".lp"; 
	}
	
	cplex.exportModel(export_file.c_str());
	

	//std::cout.setstate(std::ios::failbit);
	// Resolution time
	chrono::steady_clock sc;  
	auto start = sc.now();     // start timer

	cplex.solve();

	auto end = sc.now();       // end timer 
	auto time_span = static_cast<chrono::duration<double>>(end - start).count();   // measure time span between start & end
	
	
	//std::cout.clear();
	int errors = 0;
	if(!generated_instances){
		//~ Output result 
		for( int i = 0; i < 50 ; i++) cout << "=";
		cout << endl << "k_0 : " << k_0 << endl;
		
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
		
		
		//~ cout << "Pnt, Out | Out model " << endl;
		string tmp;
		int result, pos;
		for (int i = 0; i < k ; i++){
			getline(cfile, tmp);
			result = 1-int(abs(cplex.getValue(s[i])));
			pos = tmp.find(",");
			if( result != stoi(tmp.substr(pos+1, 1)) ){
				//~ cout << tmp << " | " << 1-int(abs(cplex.getValue(s[i]))) << endl;
				errors++;
			}
		}
		cout << "MISMATCHED RESULTS: " << errors << endl << endl;
	}
	
	string res_name;
	if(generated_instances){
		res_name = "../src/data/SFSOD/generated_results/basic_results.csv";
	}
	else{
		res_name = "../src/data/SFSOD/basic_results.csv";
	}
	fstream dest_file;
	
	string line = "";
	
	ifstream myfile;
	myfile.open(res_name);
	if(!myfile) {
		//cout<<"file not exists"<<endl;
		if(generated_instances){
			line = "Instance;d_0;k_0;Time;OurObj;intercept;slopes\n";
		}
		else{
			line = "Instance;d_0;k_0;MismatchedOutliers;OurObj;intercept;slopes\n";
		}
	} 
	
	dest_file.open(res_name, fstream::app);
	if(generated_instances){
		line += filename + ";" + to_string(d_0) + ";" + to_string(percentage) + ";" + to_string(time_span) + ";" + to_string(cplex.getValue(obj)) + ";" + to_string(cplex.getValue(z)) + ";";
	}
	else{
		line += filename + ";" + to_string(d_0) + ";" + to_string(percentage) + ";" + to_string(errors) + ";" + to_string(cplex.getValue(obj)) + ";" + to_string(cplex.getValue(z)) + ";";
	}
	for(int i=0; i<d; i++){
		if(i != d-1)
			line += to_string(cplex.getValue(a[i])) + "~";
		else
			line += to_string(cplex.getValue(a[i]));
	}
	
	dest_file<<line<<endl;
	
	cout << "Obj value: " << cplex.getValue(obj) << endl;
	
	env.end();
	cout << "Environment destroyed." << endl;

	return 0;
}


