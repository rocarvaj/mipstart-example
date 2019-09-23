#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <numeric>
#include "gurobi_c.h"

using namespace std;


int ReadSolution(string solfile,
        vector<double> &solution,
        vector<string> &names,
        double &objval);


int main(int argc, char* argv[])
{
    int status = 0;

    GRBenv *env = NULL;

    status = GRBloadenv(&env, NULL);
    assert(!status);

    //status = CPXsetintparam(env, CPXPARAM_ScreenOutput, CPX_ON);
    //assert(!status);

    GRBmodel *model;
    status = GRBreadmodel(env, argv[1], &model);
    assert(!status);


    vector<double> solution;
    vector<string> names;
    double objectivevalue;

    status = ReadSolution(argv[2],
            solution,
            names,
            objectivevalue);
    assert(!status);

    //We sort the solution, according to index in problem
    int numvars;
    status = GRBgetintattr(model, "NumVars", &numvars);
    assert(!status);
    vector<double> sortedsolution(numvars);

    for(int i = 0; i < solution.size(); ++i)
    {
        int idx;
        status = GRBgetvarbyname(model, 
                names[i].c_str(),
                &idx);
        if(idx != -1)
        {
            sortedsolution[idx] =solution[i];
            if(idx >= sortedsolution.size())
            {
                cout << "Variable index out of bounds!!" <<  endl;
                exit(1);
            }
        }
        else
        {
            cerr << "Error. Name not found" << endl;
            exit(1);
        }
    }

    //vector<int> indices;
    //int beg = 0;
    //int effortlevel = CPX_MIPSTART_CHECKFEAS;
    //for(int i = 0; i < CPXgetnumcols(env, problem); ++i)
        //indices.push_back(i);


    status = GRBsetdblattrarray(model,
            "Start",
            0,
            numvars-1,
            &(sortedsolution[0]));
    assert(!status);

    //status = CPXaddmipstarts(env,
            //problem,
            //1,
            //CPXgetnumcols(env, problem),
            //&beg,
            //&(indices[0]),
            //&(sortedsolution[0]),
            //&(effortlevel),
            //NULL);
    //assert(!status);

    ////check objective function
    //vector<double> obj(CPXgetnumcols(env, problem));

    //status = CPXgetobj(env, 
            //problem, 
            //&(obj[0]),
            //0,
            //CPXgetnumcols(env, problem)-1);
    //assert(!status);

    //cout << "Number of binary variables: " << CPXgetnumbin(env, problem) << "/" << CPXgetnumcols(env, problem) << endl;
    //cout << "Obj value of solution: " << inner_product(obj.begin(), obj.end(), sortedsolution.begin(), 0.0) << endl;

    //cout << "Sum of nonzero coefficients: " << accumulate(obj.begin(), obj.end(), 0.0) << endl;

    //status = CPXwriteprob(env, problem, "problem.lp", NULL);
    //assert(!status);

    //status = GRBread(model, argv[2]);
    //assert(!status);

    status = GRBsetdblparam(GRBgetenv(model), "TimeLimit", 10.0);
    assert(!status);

    status = GRBoptimize(model);

    string solname = argv[2];
    solname += "_grb.sol";

    status = GRBwrite(model, solname.c_str());
    assert(!status);
    status = GRBfreemodel(model);
    assert(!status);

    GRBfreeenv(env);


}

int ReadSolution(string solfile,
        vector<double> &solution,
        vector<string> &names,
        double &objval)
{
    int status;

    ifstream infile(solfile.c_str());

    string line;
    vector<string> tokens;

    while(getline(infile, line))
    {
        if(line == "")
            continue;
        boost::algorithm::split(tokens, line, boost::algorithm::is_space(), boost::token_compress_on);
        if(tokens[0] == "=obj=")
        {
            objval = atof(tokens[1].c_str());
        }
        else
        {
            solution.push_back(atof(tokens[1].c_str()));
            names.push_back(tokens[0].c_str());
            cout << "Variable " << tokens[0] << "= " << atof(tokens[1].c_str()) << endl;
        }
    }

    infile.close();

    return 0;
}

