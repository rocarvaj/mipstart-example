# Adding mip starts
This code is meant to isolate a weird behavior by CPLEX (12.7.1 and 12.8). 
When adding the optimal solution for the `markshare2` problem, provided in 
[MIPLIB2017](http://miplib.zib.de/downloads/solutions/markshare2/1/markshare2.sol.gz),
as a MIP start, CPLEX claims that no feasible solution was found from it. This does not
happen when using Gurobi.

## Requirements
* [Boost](https://www.boost.org) (just for the split function for strings).

## Compiling
1. Update paths in makefile
2. For CPLEX, use `make cpx`. For Gurobi, `make grb`.

## Usage
```    
./example{_grb} data/markshare.mps.gz data/markshare.sol
```
