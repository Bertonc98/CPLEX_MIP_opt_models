#include "../src/utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ilcplex/ilocplex.h>
#include <limits>
#include <math.h>

void compute_W(IloNumArray solution, IloNumArray wl, IloNumArray wu, double_t scale_factor){
	int len = solution.getSize();
	//~ Computing bounds
	for( int i = 1; i < len; i++ ){
		//~ Positive case: lower the half, upper the double
		if( solution[i] > 0 ){
			wu[i-1] = (float_t)solution[i]*scale_factor;
			wl[i-1] = (float_t)solution[i]-wu[i-1];
		}
		//~ Negative case: lower the double, upper the half
		else if( solution[i] < 0 ){
			wl[i-1] = (float_t)solution[i]*scale_factor;
			wu[i-1] = (float_t)solution[i]-wl[i-1];
		}
		else{
			wl[i-1] = -scale_factor;
			wu[i-1] = scale_factor;
		}
	}
	
}

void compute_W_optimal_hyperplane(IloNumArray solution, IloNumArray wl, IloNumArray wu, double_t scale_factor){
	//~ The optimal hyperplane is composed by the first 5 features set to 1, the others to 0
	//~ No bound for the last coefficient, the intercept
	std::cout << "Scale factor: " << scale_factor <<std::endl;
	int len = solution.getSize();
	//~ Computing bounds
	for( int i = 1; i < len; i++ ){
		wl[i-1] = ((float_t)solution[i] - 2) * scale_factor;
		wu[i-1] = ((float_t)solution[i] + 2) * scale_factor;
	}
	
}

float_t dot_product(IloNumArray x, IloNumArray solution){
	//The intercept is added previously
	float_t res = solution[0];
	
	//~ Dot product x * solution cohefficients
	for( int i = 0; i < x.getSize(); i++ ){
		res += x[i] * solution[i+1];
	}

	return res;
}

void compute_R(IloNumArray solution, IloNumArray2 x, IloNumArray y, IloNumArray r, double_t scale_factor){
	int solution_size =  x.getSize();
	std::cout << "R scale factor: " << scale_factor << std::endl;
	float v = 0.0;
	
	float error;
	
	for( int i = 0; i < solution_size; i++ ){		
		error = abs( y[i] - dot_product(x[i], solution) );	
			
		if(error > v){
			v = error;
		}
	}
	// SCALING OF THE BOUND 5 or 10
	v *= scale_factor;
	
	for( int i = 0; i < solution_size; i++ ){
		r[i] = std::ceil(v);
	}
}

void compute_RpRm(IloNumArray solution, IloNumArray2 x, IloNumArray y, IloNumArray rp, IloNumArray rm, double_t scale_factor){
	int solution_size =  x.getSize();
	std::cout << "R scale factor: " << scale_factor << std::endl;
	float p = 0.0, m = 0.0;
	
	float error;
	
	for( int i = 0; i < solution_size; i++ ){		
		error = y[i] - dot_product(x[i], solution);	
		if(error < 0.0){
			error = abs(error);
			if(error > m){
				m = error;
			}
		}	
		else{
			if(error > p){
				p = error;
			}
		}
	}
	// SCALING OF THE BOUND 5 or 10
	m *= scale_factor;
	p *= scale_factor;
	
	for( int i = 0; i < solution_size; i++ ){
		rp[i] = std::ceil(p);
		rm[i] = std::ceil(m);
	}
}

void debug(int i){
	std::cout << "HERE: " << i << std::endl << std::endl;
}

void print_conflicts(IloEnv env, IloModel model, IloCplex cplex){
	IloConstraintArray constraints(env);
	for (IloModel::Iterator it(model); it.ok(); ++it) {
         IloExtractable ext = *it;
         if (ext.isVariable()) {
            IloNumVar v = ext.asVariable();
            // Add variable bounds to the constraints array.
            constraints.add(IloBound(v, IloBound::Lower));
            constraints.add(IloBound(v, IloBound::Upper));
         }
         else if (ext.isConstraint()) {
            IloConstraint c = ext.asConstraint();
            constraints.add(c);
         }
      }
      
      IloNumArray prefs(env, constraints.getSize());
      for (int i = 0; i < prefs.getSize(); ++i)
         prefs[i] = 1.0;
	if (cplex.refineConflict(constraints, prefs)) {
		 // Display the solution status.
		 IloCplex::CplexStatus status = cplex.getCplexStatus();
		 std::cout << "Solution status = " << status << " (" <<
			static_cast<int>(status) << ")" << std::endl;

		 // Get the conflict status for the constraints that were specified.
		 IloCplex::ConflictStatusArray conflict = cplex.getConflict(constraints);

		 // Print constraints that participate in the conflict.
		 std::cout << "Conflict:" << std::endl;

		 for (int i = 0; i < constraints.getSize(); ++i) {
			if (conflict[i] == IloCplex::ConflictMember ||
				conflict[i] == IloCplex::ConflictPossibleMember) {
			   std::cout << "  " << constraints[i] << std::endl;
			}
		 }
	 }
}

std::ifstream input(int argc, char **argv, bool& generated_instances, int& cardinality, 
		   int& dimensionality, double_t& scale_factor, std::string& instance, IloInt& d_0, IloInt& k_0,
		   std::string& path, std::string& filename){			   
	std::cout << "Creating environment..." << std::endl;
	std::string model = argv[0];
	
	if(argc < 4){
		std::cout << "Wrong number of parameters"<<std::endl;
		std::cout << "./" + model + " instance_number d k cardinality* dimensionality*, scale_factor*"<<std::endl;
		std::cout << "d is the amount of features that are at most NOT considered"<<std::endl;
		std::cout << "k is the number of points that are at least considered as outliers" <<std::endl;
		std::cout << "instance_number is the toy_instance considered" <<std::endl;
		std::cout << "cardinality is optional. and it will lead to handle the generated instances" <<std::endl;
		std::cout << "dimensionality is optional. and it will lead to handle the generated instances" <<std::endl;
		std::cout << "scale_factor is optional. and it will lead to handle the generated instances" <<std::endl;
		exit(1);
	}
	
	if(argc == 7){
		cardinality = std::stoi(argv[4]);
		dimensionality = std::stoi(argv[5]);
		scale_factor = std::stoi(argv[6]);
		generated_instances = true;
	}
	
	instance = argv[1];
	d_0 = std::stoi(argv[2]);
	k_0 = std::stoi(argv[3]);	
	std::cout << "d_0/s_0: " << d_0 << std::endl;
	std::cout << "k_0: " << k_0 << std::endl;
	
	
	if(generated_instances){
		//remove bigmtune to toy after tuning of w
		path = "../src/instance_set/generated_instances/";
		filename  = path + "toy_" + std::to_string(cardinality) + "_" + std::to_string(dimensionality) + "_-" + instance + ".dat";
	}
	else{
		path = "../src/instance_set/";
		filename  = path + "toy_30_10_02_2_0_5_-10_" + instance + ".dat";
	}
	
	std::ifstream ifile(filename);
	if (!ifile) {
		std::cerr << "ERROR: could not open instance file '" << filename << std::endl;
		std::cout << "./" + model + " instance_number d k cardinality*"<<std::endl;
		std::cout << "d is the amount of features that are at most NOT considered"<<std::endl;
		std::cout << "k is the number of points that are at least considered as outliers" <<std::endl;
		std::cout << "instance_number is the toy_instance considered" <<std::endl;
		std::cout << "cardinality is optional. and it will lead to handle the generated instances" <<std::endl;
		exit(1);
	}
	
	return ifile;
}

std::ifstream read_solutions(int& solution_n, int& percentage, std::string& sol, bool generated_instances, 
					std::string filename, std::string path, int k_0, int d_0, IloNumArray2 x, std::string instance, int dimensionality){
	//~ Read solutions
	solution_n = 10;
	
	if(!generated_instances){	
		percentage = ((float_t)k_0 / (float_t)x.getSize())*10.0;
		sol = path + "optimal_solutions/minError_toy_30_10_02_2_0_5_-10_" + instance + 
					 "_l1_LinMgr_indicator_L0Mgr_sos1_" + std::to_string(d_0) +
					 ".000000_0." + std::to_string(percentage) + 
					 "00000Result.dat";
	}
	else{
		solution_n = dimensionality;
		sol = path + "hyperplane" + std::to_string(dimensionality) + ".dat";
	}
	
	std::ifstream sfile(sol);
	if (!sfile) {
		std::cerr << "ERROR: could not open solution file '" << sol << std::endl;
		std::cout << "./model instance_number d k"<<std::endl;
		std::cout << "d is the amount of features that are at most NOT considered"<<std::endl;
		std::cout << "k is the number of points that are at least considered as outliers" <<std::endl;
		std::cout << "instance_number is the toy_instance considered" <<std::endl;
		exit(1);
	}
	
	std::cout << "+++Working with: " << sol << std::endl;
	std::cout << "+++Instance: " << filename << std::endl;
	return  sfile;
	}

std::string save_results(std::fstream& dest_file, bool generated_instances, int dimensionality, int k, int d, double_t scale_factor, 
					int time_span, IloCplex cplex, IloNumVar z, int d_0, int percentage, int errors, 
					IloNumVarArray a, IloAlgorithm::Status st, std::string filename, std::string model){
	
	std::string res_name;
	// TODO remove the "root"
	std::string version = "uncut_root_";
	if(generated_instances){
		res_name = "../src/data/SFSOD/generated_results/" + version + model + "_results.csv";
	}
	else{
		res_name = "../src/data/SFSOD/" + model + "_results.csv";
	}
	
	
	std::string line = "";
	
	std::ifstream myfile;
	myfile.open(res_name);
	if(!myfile) {
		//cout<<"file not exists"<<std::endl;
		if(generated_instances){
			line = "Instance;d;k;w_bound;Time;OurObj;BestBound;intercept;slopes\n";
		}
		else{
			line = "Instance;d_0;k_0;MismatchedOutliers;OurObj;intercept;slopes\n";
		}
	} 
	std::cout << res_name << std::endl;
	dest_file.open(res_name, std::fstream::app);
	bool solvable = true;
	if(generated_instances){
		try{
			line += filename + ";" + std::to_string(dimensionality) + ";" + std::to_string(k) + ";" + std::to_string(scale_factor) + ";" + std::to_string(time_span) + ";" + std::to_string(cplex.getObjValue() ) + ";" + std::to_string(cplex.getBestObjValue() ) + ";" + std::to_string(cplex.getValue(z)) + ";";
		}
		catch(IloException e){
			solvable = false;
			line += filename + ";" + std::to_string(dimensionality) + ";" + std::to_string(k) + ";" + std::to_string(scale_factor) + ";" + std::to_string(time_span) + "None;None;None;";
		}
	}
	else{
		line += filename + ";" + std::to_string(d_0) + ";" + std::to_string(percentage) + ";" + std::to_string(errors) + ";" + std::to_string(cplex.getObjValue()) + ";" + std::to_string(cplex.getValue(z)) + ";";
	}
	
	for(int i=0; i<d; i++){
		if(!solvable){
			if(i != d-1)
				line += "None~";
			else
				line += "None";
		}
		else{
			if(i != d-1)
				line += std::to_string(cplex.getValue(a[i])) + "~";
			else
				line += std::to_string(cplex.getValue(a[i]));
		}
	}
	return line;	
}

void mismatching_points(int& errors, IloCplex cplex, int k_0, int d_0, int k, std::string path, std::string instance, 
						int percentage, IloNumVarArray s, IloNumVarArray f, std::string model_name, bool generated_instances, int d){
		//~ Output result 
		for( int i = 0; i < 50 ; i++) std::cout << "=";
		std::cout << std::endl << "k_0 : " << k_0 << std::endl;
		
		std::string compare;
		if(generated_instances){
			compare = path + "toy_" + std::to_string(k) + 
						 "_" + std::to_string(d+1) +
						 "_-" + instance + 
						 "Outliers.dat";
		}
		else{
			compare = path + "generated_instances/" + instance + 
						 "_l1_LinMgr_indicator_L0Mgr_sos1_" + std::to_string(d_0) +
						 ".000000_0." + std::to_string(percentage) + 
						 "00000Outlier.csv";
		}
		
		std::ifstream cfile;
		cfile.open(compare);
		if (!cfile) {
			std::cerr << "ERROR: could not open comparison file '" << compare << std::endl;
			std::cout << "./basic_model instance_number d k"<< std::endl;
			exit(1);
		}
		
		
		// std::cout << "Pnt, Out | Out model " << std::endl;
		std::string gen = "";
		std::string version = "uncut_";
		if(!generated_instances)
			gen = "optimal_solutions/";
		std::string res_file = path + gen + "results/" + version + "toy_" + std::to_string(k) + 
						 "_" + std::to_string(d) +
						 "_-" + instance + "-" +
						 model_name + "-" + 
						 "OutliersPrediction.dat";
		std::cout << res_file << std::endl;
		std::ofstream mf(res_file);
		
		std::string tmp;
		int result, pos;
		for (int i = 0; i < k ; i++){
			std::getline(cfile, tmp);
			//std::cout<<i<<std::endl;
			result = 1-int(abs(cplex.getValue(s[i])));
			pos = tmp.find(",");
			if( result != std::stoi(tmp.substr(pos+1, 1)) ){
				if(generated_instances){
					mf << tmp << " | " << 1-int(abs(cplex.getValue(s[i]))) << std::endl;
				}
				errors++;
			}
		}
		
		res_file = path + gen + "results/" + version + "toy_" + std::to_string(k) + 
						 "_" + std::to_string(d) +
						 "_-" + instance + "-" +
						 model_name + "-" +
						 "FeaturesPrediction.dat";
					
		std::ofstream ffile(res_file);
		for (int i = 0; i < d ; i++){
			//std::cout<<i<<std::endl;
			if(generated_instances){
				//ffile << int(abs(cplex.getValue(f[i]))) << std::endl;
				ffile << cplex.getValue(f[i]) << std::endl;
			}
		}
		
		
		std::cout << "MISMATCHED RESULTS: " << errors << std::endl << std::endl;
}
