#include "../src/utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>	
#include <list>
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
	
	IloNumArray2 x(env);
	
	x.add(IloNumArray(env, 3));
	x.add(IloNumArray(env, 3));
	
	for(int i =0; i<2; i++){
		for(int j=0; j<3; j++){
			x[i][j] = 7;
		}
	}
	
	cout << x <<endl;
	
	ifstream infile("../src/data/node_aggressive0.lp");
	
	string current_line;
	list<string> constraints;
	string constraint = "";
	bool parsing = false;
	while (std::getline(infile, current_line)){
		istringstream iss(current_line);
		if(current_line.size() <= 1)
			continue; 
			
		if(current_line.at(1) == 'L'){
			
			parsing = true;
			constraints.push_back(constraint);
			//cout << constraint << endl;
			constraint = current_line;
		}
		else if(current_line.at(1) == ' ' && parsing){
			constraint += current_line;
		}
		else if(current_line.at(1) != ' ' && current_line.at(1) != 'L'){
			if(parsing)
				constraints.push_back(constraint);
			parsing = false;
		}

	}
	constraints.pop_front();
	
	//cout << "=========================="<<endl;
	
	//open file for writing
	ofstream fw("../src/data/constraints.txt", ofstream::out);
	
	//check if file was successfully opened for writing
	if (fw.is_open()){
	  //store array contents to text file
	  for (auto c : constraints)
        fw << c << "\n";
	  fw.close();
	}
	else cout << "Problem with opening file";
	
	
	

	return 0;
}


