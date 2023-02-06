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
 
void parse_constraint(IloNumVar G, int n_variables);

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
	IloInt s_0 = stoi(argv[3]);
	

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
		Rp[i] *= 100000;
		Rm[i] *= 100000;
	} 
	
	// RUN CONSTRAIN EXTRACTION OF node_aggressive0.lp
	// Amount of variables
	// w, z, pp, pm, f, s
	int n_variables = d + 1 + k + k + d + k;
	
	// Matrix GAMMA of contraint coefficients
	
	// For each constraint add a new IloNumArray of n_varaibles length
	filename  = "../src/data/GAMMA.txt";
	ifstream Gfile(filename);
	if (!Gfile) {
		cerr << "ERROR: could not open instance file 'GAMMA.txt' in '../src/data'" << endl;
		return 1;
	}
	
	//~ Read instances
	IloNumArray2 GAMMA(env);
	int G_row;
	Gfile >> GAMMA >> G_row;
	
	//~ END OF PREPROCESSING
	
	//~ MODEL DEFINITION
	IloModel model(env);
	
	//~ Variable Definition (49)
	// alpha lenght I (dual of (37, 38))
	IloNumVarArray ap(env, k, 0, IloInfinity); 
	ap.setNames("a+");
	IloNumVarArray am(env, k, 0, IloInfinity); 
	am.setNames("a-");
	
	// pi length I (dual of (39, 40))
	IloNumVarArray pip(env, k, 0, IloInfinity); 
	pip.setNames("pi+");
	IloNumVarArray pim(env, k, 0, IloInfinity); 
	pim.setNames("pi-");
	
	// psi length I (dual of(39, 40))
	IloNumVarArray psip(env, k, 0, IloInfinity); 
	psip.setNames("psi+");
	IloNumVarArray psim(env, k, 0, IloInfinity); 
	psim.setNames("psi-");
	
	// lambdaU length J (dual of (41))
	IloNumVarArray lu(env, d, 0, IloInfinity); 
	lu.setNames("lambdaU");
	
	// lambdaL length J (dual of (42))
	IloNumVarArray ll(env, d, 0, IloInfinity); 
	ll.setNames("lambdaL");
	
	// Beta1, Beta2 (dual of (43, 44))
	IloNumVar b1(env, 0, IloInfinity);
	b1.setName("beta1");
	IloNumVar b2(env, 0, IloInfinity);
	b2.setName("beta2");
	
	// eta length I (dual of (45))
	IloNumVarArray etap(env, k, 0, IloInfinity); 
	etap.setNames("eta+");
	IloNumVarArray etam(env, k, 0, IloInfinity); 
	etam.setNames("eta-");
	
	// phi1, phi2 length J (dual of (46))
	IloNumVarArray phi1(env, k, 0, IloInfinity); 
	phi1.setNames("phi1");
	IloNumVarArray phi2(env, k, 0, IloInfinity); 
	phi2.setNames("phi2");
	
	// sigma1, sigma2 length I (dual of (47))
	IloNumVarArray sigma1(env, k, 0, IloInfinity); 
	sigma1.setNames("sigma1");
	IloNumVarArray sigma2(env, k, 0, IloInfinity); 
	sigma2.setNames("sigma2");
	
	// Xi length G_row (dual of inserted cosntraints in GAMMA)
	IloNumVarArray xi(env, G_row, 0, IloInfinity); 
	xi.setNames("xi");
	

	//Violation cost
	//IloNumVar C(env, 100);
	int C = 10;
	//Confidence region parameter
	//IloNumVar eps(env, 0);
	int eps = 10;
	
	//~ Objective Function (48)
	
	IloObjective obj(env);
	
	IloExpr obj_expr(env);
	
	IloExpr obj_tmp(env);
	// Over dimensionality
	for(int j =0; j < d; j++){
		// Over cardinality
		for(int i=0; i < k; i++){
			obj_tmp += x[i][j] * (am[i] - ap[i]);
		}
		
		obj_expr += IloSquare(obj_tmp);		
	}
	obj_expr *= 1/2;
	cout << obj_tmp <<endl;
	
	obj.setExpr(obj_expr);
	obj_expr.end();
	
	IloAdd(model, IloMinimize(env, obj));
	model.add(xi[0] >= 10);
	/*
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
	*/
	IloCplex cplex(env);
	//~ cplex.add(obj_expr);
	
	cplex.extract(model);
	
	string export_file = "../src/data/dual_models/dual_model"+ instance + "_" + to_string(d_0) + "_" + to_string(k_0) + ".lp"; 
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
		cout << "./dual_model instance_number d k s"<<endl;
		return 1;
	}
	// CONFLICTS //
	//If not feasible
	if(st != 2)
		print_conflicts(env, model, cplex);

	// 
	
	/*
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
			cout << tmp << " | " << int(abs(cplex.getValue(s[i]))) << endl;
			errors++;
		}
	}
	//~ Saving results
	fstream dest_file;
	string res_name = "../src/data/dual_results.csv";
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

void parse_constraint(IloNumVar G, int n_variables){
	ifstream infile("../src/data/note_aggressive0.lp");
	
	string line;
	while (std::getline(infile, line)){
		istringstream iss(line);
		// Parse line
		cout << line << "---" << endl;
	}
}


