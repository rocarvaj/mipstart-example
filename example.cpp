#include "cplex.h"
#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <numeric>

using namespace std;


int ReadSolution(string solfile,
        vector<double> &solution,
        vector<string> &names,
        double &objval);


int main(int argc, char* argv[])
{
    int status = 0;

    CPXENVptr env = CPXopenCPLEX(&status);
    assert(!status);

    cout << "CPLEX version: " <<  CPXversion(env) << endl;

    status = CPXsetintparam(env, CPXPARAM_ScreenOutput, CPX_ON);
    assert(!status);

    CPXLPptr problem = CPXcreateprob(env, &status, "Test problem");
    assert(!status);

    status = CPXreadcopyprob(env, 
        problem, 
        argv[1], 
        NULL);
    assert(!status);


    vector<double> solution;
    vector<string> names;
    double objectivevalue;

    status = ReadSolution(argv[2],
            solution,
            names,
            objectivevalue);
    assert(!status);

    // We sort the solution, according to index in problem
    vector<double> sortedsolution(CPXgetnumcols(env, problem));

    for(int i = 0; i < solution.size(); ++i)
    {
        int idx;
        status = CPXgetcolindex(env, 
                problem, 
                names[i].c_str(),
                &idx);
        if(&idx != NULL)
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

    vector<int> indices;
    int beg = 0;
    int effortlevel = CPX_MIPSTART_CHECKFEAS;
    for(int i = 0; i < CPXgetnumcols(env, problem); ++i)
        indices.push_back(i);

    status = CPXaddmipstarts(env,
            problem,
            1,
            CPXgetnumcols(env, problem),
            &beg,
            &(indices[0]),
            &(sortedsolution[0]),
            &(effortlevel),
            NULL);
    assert(!status);

    status = CPXsetdblparam(env, CPXPARAM_TimeLimit, 10.0);
    assert(!status);

    status = CPXmipopt(env, problem);

    status = CPXfreeprob(env, &problem);
    assert(!status);

    status = CPXcloseCPLEX(&env);
    assert(!status);


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
        boost::algorithm::split(tokens, 
                line, 
                boost::algorithm::is_space(), 
                boost::token_compress_on);
        if(tokens[0] == "=obj=")
        {
            objval = atof(tokens[1].c_str());
        }
        else if(tokens[0] != "#")
        {
            solution.push_back(atof(tokens[1].c_str()));
            names.push_back(tokens[0].c_str());
            //cout << "Variable " << tokens[0] << "= " << atof(tokens[1].c_str()) << endl;
        }
    }

    infile.close();

    return 0;
}
