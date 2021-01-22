#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <iomanip>
#include <chrono>
#include <cstdlib>
#include <limits>
#include <bits/stdc++.h> 

#include "Classes/Multiway1Solver.h"
#include "Classes/MultiwayAlternativeSolver.h"
#include "Classes/MultiwayMixSolver.h"
#include "Classes/MultiwaySortSolver.h"

int DEBUG = 0;
int DEBUG_RESULT = false;

using namespace std;
using namespace lemon;

void verifications(int argc, char** argv)
{
    if(argc < 3)
    {
        cout << "This code requires a type of solver and a instance input." << endl << endl;
        cout << "In the first argument you choose which algorithm you want to run:" << endl;
        cout << "0: Original min cut based algorithm (Multiway1Solver)" << endl;
        cout << "1: Alternative (MultiwayAlternativeSolver)" << endl;
        cout << "2: Mixed (MultiwayMixSolver)" << endl;
        cout << "3: Sorted (MultiwaySortSolver)" << endl << endl;

        cout << "The second argument is the instance. Please select one from the instances directory." << endl;
        cout << "Example of usage: "<< argv[0] << " 0 steinb1.txt" << endl;
        exit(0);
    }

    if(stoi(argv[1]) < 0 || stoi(argv[1]) > 3)
    {
        cout << "Invalid solver." << endl;
        exit(0);
    }

    if((argv[1] == string("1")) && (argc < 4))
    {
        cout << "When running MultiwayAlternative, you need to choose if you want to run it in a random order of terminals." << endl;
        cout << "This will be the third argument:" << endl;
        cout << "0 if you want to run with the original order. 1 if in random order" << endl;
        cout << "Example of usage: "<< argv[0] << " 1 steinb1.txt 0" << endl;
        exit(0);
    }

    if (argc > 3)
        if((argv[3] != string("0")) && (argv[3] != string("1")))
        {
        cout << "This algorithm requires a BOOL parameter indicating if you want to run the min cuts in a random order." << endl;
        cout << "0 if you want to run with the original order. 1 if in random order" << endl;
        cout << "Example of usage: "<< argv[0] << " 1 steinb1.txt 0" << endl;
        exit(0);
        }
}

int main(int argc, char** argv)
{
    verifications(argc, argv);

    int algorithm = stoi(argv[1]);

    // getting file name
    string file_name = argv[2];

    // instantiating graph, arc map for edge costs, and vector of terminals
    ListDigraph digraph;
    ListDigraph::ArcMap<int> capacity(digraph);
    vector<int> terminals;

    // start counting time
    auto start = chrono::steady_clock::now();

    // Multiway1 Solver
    if (algorithm == 0)
    {
        // instantiating Multiway1 Solver object
        Multiway1Solver solver(DEBUG);

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
        vector< vector <tuple <int, int> > > multiway_cut;

        solver.get_multiway_cut(digraph, capacity, terminals, multiway_cut);
        set<pair<int, int>> cut_set;

        int multiway_cut_cost = solver.calculate_cost_and_get_list(digraph, capacity, multiway_cut, cut_set);

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

        solver.save_result_in_file(file_name, cut_set, multiway_cut_cost, time_taken);
        return(0);
    }

    // Mutiway3 (Alternative) Solver
    // RIGHT now it is only working with the shuffle
    if (algorithm == 1)
    {
        // instantiating Multiwaycut Alternative (multiway3) Solver object 
        MultiwayAlternativeSolver solver(DEBUG);

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
        vector< vector <tuple <int, int, int> > > multiway_cut;

        set<tuple<int, int, int>> cut_set;
        set<tuple<int, int, int>> best_cut_set;
        int multiway_cut_cost;
        int best_result = numeric_limits<int>::max();

        // vector terminals_order determinates the order in which the algorithm will calculate min cuts.
        // the first min cut will be calculated for terminals[terminals_order[0]] vertex
        // so, for all x in terminals_order, 0 < x < terminals.size() 
        vector<int> terminals_order;      
        
        // shuffle
        bool shuffle = (argv[3] == string("1"));

        for (int i = 0; i < terminals.size(); i++)
            terminals_order.push_back(i);

        if (shuffle == true)
        {
            for (int k=0; k < 20; k++)
            {

                srand (time(NULL));  
                for (int i = terminals.size() - 1; i > 0; i--)  
                {  
                    // Pick a random index from 0 to i  
                    int j = rand() % (i + 1);  
            
                    // Swap arr[i] with the element at random index  
                    swap(terminals_order[i], terminals_order[j]);  
                }  

                solver.get_multiway_cut(digraph, capacity, terminals, terminals_order, multiway_cut);

                multiway_cut_cost = solver.calculate_cost_and_get_list(digraph, capacity, multiway_cut, cut_set);
                
                if (multiway_cut_cost < best_result)
                {
                    best_result = multiway_cut_cost;
                    best_cut_set = cut_set;
                }
                
                // cleading our data structures
                multiway_cut.clear();
                cut_set.clear();
                solver.read_file(digraph, file_name, capacity, terminals);
            }
        }
      
        if (DEBUG_RESULT == true)
        {
            cout << "---------" << endl << "Solution: " << endl;
            solver.printing_cut_list(best_cut_set);
            cout << "Cost of multiway cut: " << best_result << endl;
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
        solver.save_result_in_file(file_name, best_cut_set, best_result, time_taken);

        return(0);
    }

    // Multiway Mix Solver
    if (algorithm == 2)
    {
        // instantiating Multiwaycut Alternative Solver object
        MultiwayMixSolver solver(DEBUG);

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
        vector< vector <tuple <int, int, int> > > multiway_cut;

        solver.get_multiway_cut(digraph, capacity, terminals, multiway_cut);
        set<tuple<int, int, int>> cut_set;

        int multiway_cut_cost = solver.calculate_cost_and_get_list(multiway_cut, cut_set);

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

        solver.save_result_in_file(file_name, cut_set, multiway_cut_cost, time_taken);

        return(0);
    }

    // Multiway Sort Solver
    if (algorithm == 3)
    {
        // instantiating Multiwaycut Sort Solver object
        MultiwaySortSolver solver(DEBUG);

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
        vector< vector <tuple <int, int, int> > > multiway_cut;

        solver.get_multiway_cut(digraph, capacity, terminals, multiway_cut);
        set<tuple<int, int, int>> cut_set;

        int multiway_cut_cost = solver.calculate_cost_and_get_list(multiway_cut, cut_set);

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

        solver.save_result_in_file(file_name, cut_set, multiway_cut_cost, time_taken);

        return(0);

    }

    cout << "?????? que??" << endl;
    return 0;
}
