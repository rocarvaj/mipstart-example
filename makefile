CPX_PATH = /apps/cplex128/cplex
#CPX_PATH = /home/rocarvaj/ILOG/CPLEX_Studio1271/cplex
BOOST_PATH = /home/rocarvaj/boost_1_71_0
GRB_PATH = /apps/gurobi/

CC = g++
ARCH = x86-64_linux

cpx: 
	$(CC) -g -L $(CPX_PATH)/lib/$(ARCH)/static_pic/ -L lib/ -I $(CPX_PATH)/include/ilcplex/ -I $(BOOST_PATH) -o example example.cpp -lm -lpthread -lcplex -ldl

grb: 
	$(CC) -g -L $(GRB_PATH)/linux64/lib -I $(GRB_PATH)/linux64/include -I $(BOOST_PATH) -o example_grb example_grb.cpp -lgurobi81
