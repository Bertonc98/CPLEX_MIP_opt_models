#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main(int argc, char** argv){
	
   if(argc != 2){
      cout << "Incorrect number of arguments"<<endl;
      exit(1);
   }
   string make_structure = "make_structure.txt";
   fstream source_file;
   source_file.open(make_structure);
   
   string make_res = "Makefile";
   fstream dest_file;
   dest_file.open(make_res);
   if(dest_file.fail()){
	//~ cout<<"Destination file not exists: creating..."<<endl;
	ofstream create_file(make_res);
	create_file<<"";
	create_file.close();
   }
   dest_file.open(make_res);
   
   string exe = argv[1];
   string obj = exe + ".o";
   
   size_t exe_pos = -1;
   size_t obj_pos = -1;
   //cout<<(exe_pos != string::npos)<<endl;
   int i = 0;
   if (source_file.is_open() && dest_file.is_open()){   //checking whether the file is open
      string line;
      while(getline(source_file, line)){ 
		 i++;
		 exe_pos = line.find("EXENAME");
		 obj_pos = line.find("OBJECT");
		 
		 if(exe_pos != string::npos){
			line.erase(exe_pos, line.size()-exe_pos);
			line.replace(exe_pos, exe.size(), exe);
		 }
		 else if(obj_pos != string::npos){
			line.erase(obj_pos, line.size()-obj_pos);
			line.replace(obj_pos, obj.size(), obj);
		 }
         //cout << line << endl; 
         dest_file<<line<<endl;
         exe_pos = -1;
         obj_pos = -1;
      }
      source_file.close(); 
      dest_file.close();
      
      string command = "make ";
      command = command.append(exe);
      
      system(command.c_str());
      
      system("rm Makefile");
   }
   else if(!source_file.is_open()){
	cout<<"Source file not found"<<endl;   
   }
   else if(!dest_file.is_open()){
	cout<<"Destination file not found"<<endl;   
   }
	
	
}
