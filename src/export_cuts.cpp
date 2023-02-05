#include <sstream>
#include <iostream>
#include <fstream>
#include <string>	
#include <list>

using namespace std;

int main(int argc, char **argv){
	
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


