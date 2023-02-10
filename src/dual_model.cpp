#include "../src/utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>	
#include <cstdlib>
#include <map>
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
	
	// k = I, amount of instances
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
	map<string, int> var_pos = {
		{ "w", 0 },
		{ "z", d },
		{ "pp", d + 1 },
		{ "pm", d + 1 + k },
		{ "f", d + 1 + k + k },
		{ "s", d + 1 + k + k + d }
	};
	
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
	IloNumArray g(env);
	int G_row;
	int valid;
	Gfile >> GAMMA >> g >> G_row >> valid;
	cout << GAMMA << endl;
	cout << g << endl;
	cout << "GAMMA rows: " << G_row << endl;
	cout << "GAMMA validity: " << valid << endl;
	// In case of no cosntraints
	if(valid == 0){
		cout << "No valid cuts" <<endl;
		G_row = 1;
		g[0] = IloInfinity;
	}
	cout << "GAMMA rows: " << G_row << endl;
	cout << "bounds: " << g << endl;
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
	
	// phi length J (dual of (46))
	IloNumVarArray phi(env, d, 0, IloInfinity); 
	phi.setNames("phi");
	
	// sigma ength I (dual of (47))
	IloNumVarArray sigma(env, k, 0, IloInfinity); 
	sigma.setNames("sigma");
	
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
	// The objective function is composed by 10 terms
	// It is printed and marked each one in the following code
	IloExprArray quadratic(env, d);
	
	for(int j =0; j < d; j++){
		quadratic[j] = IloExpr(env);
		for(int i=0; i < k; i++){
			 quadratic[j] += (x[i][j] * (am[i] - ap[i]));
		}
	}
	obj_expr = (IloExpr)(IloScalProd(quadratic, quadratic));
	obj_expr *= 0.5;
	cout << "1st" <<endl;
	
	IloExprArray xiGamma(env, d);
	IloExprArray xalpha(env, d);
	for(int j =0; j < d; j++){
		xiGamma[j] = IloExpr(env);
		for(int h=0; h < G_row; h++){
			 xiGamma[j] += (xi[h] * GAMMA[h][var_pos["w"] + j]);
		}
		
		xalpha[j] = IloExpr(env);
		for(int i=0; i < k; i++){
			 xalpha[j] += (x[i][j] * (am[i] - ap[i]));
		}
		
		obj_expr += ( (ll[j] - lu[j] - xiGamma[j]) * xalpha[j] );
	}
	cout << "2nd" <<endl;
	
	IloExprArray xiGamma2(env, d);
	IloExprArray quadratic2(env, d);
	for(int j =0; j < d; j++){
		xiGamma2[j] = IloExpr(env);
		for(int h=0; h < G_row; h++){
			 xiGamma2[j] += (xi[h] * GAMMA[h][var_pos["w"] + j]);
		}
		
		quadratic2[j] = IloExpr(env);
		quadratic2[j] += ll[j] - lu[j] - xiGamma2[j];
	}
	obj_expr = (IloExpr)(IloScalProd(quadratic2, quadratic2));
	obj_expr *= 0.5;
	cout << "3th" <<endl;
	
	for(int i=0; i<k; i++){
		obj_expr -= (ap[i]*(y[i]+eps) - am[i]*(y[i]-eps));
	}
	cout << "4th" <<endl;
	
	for(int i=0; i<k; i++){
		obj_expr -= ( (pip[i] - psip[i])*Rp[i] + (pim[i] - psim[i])*Rm[i] );
	}
	cout << "5th" <<endl;
	
	obj_expr -= (k_0 * b1);
	cout << "6th" <<endl;
	
	obj_expr += (s_0 * b2);
	cout << "7th" <<endl;
	
	for(int j=0; j<d; j++){
		obj_expr -= phi[j];
	}
	cout << "8th" <<endl;
	
	for(int i=0; i<k; i++){
		obj_expr -= sigma[i];
	}
	cout << "9th" <<endl;
	
	for(int h=0; h<G_row; h++){
		obj_expr -= xi[h]*g[h];
	}
	cout << "10th" <<endl;
	
	
	obj.setExpr(obj_expr);
	obj_expr.end();
	
	IloAdd(model, IloMinimize(env, obj));
	
	
	// Constraint section
	IloExpr xiGammaZ(env);
	for(int h=0; h<G_row; h++){
		xiGammaZ += xi[h]*GAMMA[h][var_pos["z"]];
	}
	
	model.add( IloSum(ap) - IloSum(am) +  xiGammaZ == 0);
	cout << "Constraint 1" <<endl;
	
	IloExprArray xiGammaPp(env, k);
	for(int i=0; i<k; i++){
		xiGammaPp[i] = IloExpr(env);
		for(int h = 0; h < G_row; h++){
			xiGammaPp[i] += xi[h] * GAMMA[h][var_pos["pp"] + i];
		}
		
		model.add( psip[i] + ap[i] - pip[i] - xiGammaPp[i] <= C );
	}
	cout << "Constraint 2" <<endl;
	
	IloExprArray xiGammaPm(env, k);
	for(int i=0; i<k; i++){
		xiGammaPm[i] = IloExpr(env);
		for(int h = 0; h < G_row; h++){
			xiGammaPm[i] += xi[h] * GAMMA[h][var_pos["pm"] + i];
		}
		
		model.add( psim[i] + am[i] - pim[i] - xiGammaPm[i] <= C );
	}
	cout << "Constraint 3" <<endl;
	
	IloExprArray xiGammaF(env, d);
	for(int j=0; j<d; j++){
		xiGammaF[j] = IloExpr(env);
		for(int h = 0; h < G_row; h++){
			xiGammaF[j] += xi[h] * GAMMA[h][var_pos["f"] + j];
		}
		
		model.add( (ll[j] * wl[j]) - (lu[j] * wu[j]) + phi[j] + b1 + xiGammaF[j] >= 0);
	}
	cout << "Constraint 4" <<endl;
	
	IloExprArray xiGammaS(env, k);
	for(int i=0; i<k; i++){
		xiGammaS[i] = IloExpr(env);
		for(int h = 0; h < G_row; h++){
			xiGammaS[i] += xi[h] * GAMMA[h][var_pos["s"] + i];
		}
		
		model.add( (psip[i] * Rp[i]) + (psim[i] * Rm[i]) + b2 - sigma[i] - xiGammaS[i] <= (C * (Rp[i] + Rm[i])) );
	}
	cout << "Constraint 5" <<endl;
	
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
	cout << "d_0 = " << d_0 << endl;
	cout << "s_0 = " << s_0 << endl;
	
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
	cout << st << endl;
	if(st != 2)
		print_conflicts(env, model, cplex);
	else{
		cout << "Obj value: " << cplex.getObjValue() << endl;
	}	
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


