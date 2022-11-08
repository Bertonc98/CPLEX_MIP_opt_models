gcc -c -m64 -O3 -g -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD -w -I/opt/ibm/ILOG/CPLEX_Studio221/cplex/include -I/opt/ibm/ILOG/CPLEX_Studio221/concert/include -I../src/include ../src/cut_extraction.c -o ../src/cut_extraction.o

gcc -L/opt/ibm/ILOG/CPLEX_Studio221/cplex/lib/x86-64_linux/static_pic -L/opt/ibm/ILOG/CPLEX_Studio221/concert/lib/x86-64_linux/static_pic  ../src/cut_extraction.o -lconcert -lilocplex -lcplex -lm -lpthread -ldl -lstdc++fs -o cut_extraction
