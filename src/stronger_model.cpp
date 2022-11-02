#include <sstream>
#include <string>
#include <cstdlib>
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

static void compute_W(IloNumArray solution, IloNumArray wl, IloNumArray wu, int scale_factor);

static float_t dot_product(IloNumArray x, IloNumArray wu);

static void compute_R(IloNumArray solution, IloNumArray2 x, IloNumArray y, IloNumArray r);

static void debug(int i);

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
	IloNumArray r(env, k);
	
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
	IloNumVarArray p(env, k, -IloInfinity, IloInfinity); // (28)
	p.setNames("p");
	IloNumVarArray t(env, k, -IloInfinity, IloInfinity); // (29)
	t.setNames("t");
	IloNumVarArray s(env, k, 0, 1, ILOINT); // (30)
	s.setNames("s");
	
	//~ IloNumArray mones(env, k);
	//~ IloNumArray ones(env, k);
	//~ for (int i=0; i< k; i++){
		 //~ mones[i] = -1;
		 //~ ones[i] = 1;
	//~ }
	
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
		model.add(p[i] >= ((1-s[i]) * r[i])); // (17)
		model.add(p[i] <= r[i]*2); // (18) Increased bound from r[i]
		
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
	cplex.exportModel("model.lp");
	cplex.solve();
	cplex.exportModel("model_solved.lp");
	
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
	
	
	cout << "Pnt, Out | Out model " << endl;
	string tmp;
	for (int i = 0; i < k ; i++){
		getline(cfile, tmp);
		cout << tmp << " | " << 1-int(abs(cplex.getValue(s[i]))) << endl;
	}
	cout << "Obj value: " << cplex.getObjValue() << endl;
	
	env.end();
	cout << "Environment destroyed." << endl;

	return 0;
}

static void compute_W(IloNumArray solution, IloNumArray wl, IloNumArray wu, int scale_factor){
	int len = solution.getSize()-1;
	//~ Computing bounds
	for( int i = 1; i < len; i++ ){
		//~ Positive case: lower the half, upper the double
		if( solution[i] > 0 ){
			wl[i-1] = (float_t)solution[i]/scale_factor;
			wu[i-1] = (float_t)solution[i]*scale_factor;
		}
		//~ Negative case: lower the double, upper the half
		else if( solution[i] < 0 ){
			wl[i-1] = (float_t)solution[i]*scale_factor;
			wu[i-1] = (float_t)solution[i]/scale_factor;
		}
		else{
			wl[i-1] = -scale_factor;
			wu[i-1] = scale_factor;
		}
	}
	
}

static float_t dot_product(IloNumArray x, IloNumArray solution){
	//The intercept is added previously
	float_t res = solution[0];
	
	//~ if( x.getSize() != solution.getSize() )
		//~ std::length_error("Point features must be one less than the solution!");

	//~ Dot product x * solution cohefficients
	for( int i = 0; i < x.getSize(); i++ ){
		res += x[i] * solution[i+1];
	}

	return res;
}

static void compute_R(IloNumArray solution, IloNumArray2 x, IloNumArray y, IloNumArray r){
	int solution_size =  x.getSize();
	
	for( int i = 0; i < solution_size; i++ ){
		r[i] = abs( y[i] - dot_product(x[i], solution) );		
	}
}

static void debug(int i){
	cout << "HERE: " << i << endl << endl;
}
