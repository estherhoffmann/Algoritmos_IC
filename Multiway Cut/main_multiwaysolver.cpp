#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <iomanip>
#include <chrono>
#include <cstdlib>
#include <bits/stdc++.h> 

//#include "Classes/Multiway1Solver.h"
#include "Classes/MultiwayAlternativeSolver.h"
//#include "Classes/MultiwayMixSolver.h"

int DEBUG = 0;
int DEBUG_RESULT = false;

using namespace std;
using namespace lemon;

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        cout << "This code requires a instance input, and a bool parameter indicating if you want to run the min cuts in a random order" << endl;
        cout << "0 if you want to run with the original order. 1 if in random order" << endl;
        cout << "Please select one from the instances directory. (i.e. ./multiway1 steinb1.txt 0)" << endl;
        return 0;
    }

    if((argv[2] != string("0")) && (argv[2] != string("1")))
    {
        cout << "This code requires a BOOL parameter indicating if you want to run the min cuts in a random order" << endl;
        cout << "0 if you want to run with the original order. 1 if in random order" << endl;
        cout << "Please select one from the instances directory. (i.e. ./multiway1 steinb1.txt 0)" << endl;
        return 0;
    }

    // 'start counting time'
    auto start = chrono::steady_clock::now();

    // instantiating Multiwaycut Solver object
    //MultiwaySolver solver(DEBUG);

    // instantiating Multiwaycut Alternative (multiway3) Solver object 
    MultiwayAlternativeSolver solver(DEBUG);

    // instantiating Multiwaycut Alternative Solver object
    //MultiwayMixSolver solver(DEBUG);


    // instantiating graph, arc map for edge costs, and vector of terminals
    ListDigraph digraph;
    ListDigraph::ArcMap<int> capacity(digraph);
    vector<int> terminals;

    // getting file name
    string file_name = argv[1];

    if(!solver.read_file(digraph, file_name, capacity, terminals))
    {
        cout << "Could not open the file." << endl;
        return 0;
    }

    if (DEBUG >= 1)
    {
        solver.printing_graph(digraph, capacity, terminals);
    }

    //vector of (vector of tuples) that storages the edges of the cut on each iteration
    //vector< vector <tuple <int, int> > > multiway_cut;
    vector< vector <tuple <int, int, int> > > multiway_cut;

    // vector terminals_order determinates the order in which the algorithm will calculate min cuts.
    // the first min cut will be calculated for terminals[terminals_order[0]] vertex
    // so, for all x in terminals_order, 0 < x < terminals.size() 
    vector<int> terminals_order;

    for (int i = 0; i < terminals.size(); i++)
        terminals_order.push_back(i);
    

    // shuffle
    bool shuffle = (argv[2] == string("1"));
    if(shuffle == true)
    {
        srand (time(NULL));  
        for (int i = terminals.size() - 1; i > 0; i--)  
        {  
            // Pick a random index from 0 to i  
            int j = rand() % (i + 1);  
    
            // Swap arr[i] with the element at random index  
            swap(terminals_order[i], terminals_order[j]);  
        }  
    }
    
    solver.get_multiway_cut(digraph, capacity, terminals, terminals_order, multiway_cut);

    //set<pair<int, int>> cut_set;
    set<tuple<int, int, int>> cut_set;
    int multiway_cut_cost = 0;
    multiway_cut_cost = solver.calculate_cost_and_get_list(digraph, capacity, multiway_cut, cut_set);
    //multiway_cut_cost = solver.calculate_cost_and_get_list(multiway_cut, cut_set);


    if (DEBUG_RESULT == true)
    {
        cout << "---------" << endl << "Solution: " << endl;
        solver.printing_multiway_vector(multiway_cut);
        solver.printing_cut_list(cut_set);
        cout << "Cost of multiway cut: " << multiway_cut_cost << endl;
        cout << endl;
    }

    // Calculating total time taken by the program.
    auto end = chrono::steady_clock::now();
    chrono::duration<double> diff = end - start;
    double time_taken = diff.count();
    cout << "Time taken by program is : " << fixed
         << fixed << setprecision(2) << time_taken;
    cout << " sec " << endl;

    // saving results
    if(shuffle == false)
        solver.save_result_in_file(file_name, cut_set, multiway_cut_cost, time_taken);

    return 0;
}
