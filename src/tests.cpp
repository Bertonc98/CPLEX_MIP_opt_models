#include "../src/utils.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>	
#include <list>
#include <cstdlib>
#include <map>
#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

/*
 * From "Modelli MPHero the Disjunctive Linearization of the Projected McCormick's model
 * 
 */

int main(int argc, char **argv){
	// w, z, pp, pm, f, s
	// int n_variables = d + 1 + k + k + d + k;
	int d = 1;
	int k = 2;
	map<string, int> my_map = {
    { "w", 0 },
    { "z", d },
    { "pp", d + 1 },
    { "pm", d + 1 + k },
    { "f", d + 1 + k + k },
    { "s", d + 1 + k + k + d }
};

	cout << my_map["w"] <<endl;
	cout << my_map["z"] <<endl;
	cout << my_map["pp"] <<endl;
	cout << my_map["pm"] <<endl;
	cout << my_map["f"] <<endl;
	cout << my_map["s"] <<endl;
	

	return 0;
}


