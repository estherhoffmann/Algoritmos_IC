#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <iomanip>
#include <chrono>
#include "MultiwaySolver.h"

int DEBUG = 0;
int DEBUG_RESULT = false;

using namespace std;
using namespace lemon;

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cout << "This code requires a instance input. ";
        cout << "Please select one from the instances directory. (i.e. ./multiway1 steinb1.txt)";
        return 0;
    }

    // 'start counting time'
    auto start = chrono::steady_clock::now();

    // instantiating Multiwaycut Solver object
    MultiwaySolver solver(DEBUG);

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
    vector< vector <tuple <int, int> > > multiway_cut;
    solver.get_multiway_cut(digraph, capacity, terminals, multiway_cut);

    set<pair<int, int>> cut_set;
    int multiway_cut_cost = 0;
    multiway_cut_cost = solver.calculate_cost_and_get_list(digraph, capacity, multiway_cut, cut_set);

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
    solver.save_result_in_file(file_name, cut_set, multiway_cut_cost, time_taken);

    return 0;
}
