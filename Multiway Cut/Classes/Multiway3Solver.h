#ifndef MULTIWAY3SOLVER_H
#define MULTIWAY3SOLVER_H
#include <iostream>
#include <vector>
#include <string>
#include <set>

#include <lemon/list_graph.h>
#include <lemon/concepts/maps.h>
#include <lemon/concepts/digraph.h>

using namespace lemon;
using namespace std;

class Multiway3Solver {
private:
    // variable for debugging
    int DEBUG;

    // functions that can be only used inside our public functions
    int position_highest_value_in_vector(vector<int>&);  
    void update_multiwaycut(ListDigraph&, ListDigraph::NodeMap<bool>&, ListDigraph::ArcMap<int> &, vector<vector<tuple <int, int, int>>>&);

public:

	Multiway3Solver(int);
	~Multiway3Solver();

    void printing_graph(ListDigraph&, ListDigraph::ArcMap<int>&, vector<int>&);
       
    // read graph file and saves it in our graph structure
    bool read_file(ListDigraph&, string, ListDigraph::ArcMap<int>&, vector<int>&);

    // stores in cut_set the cut edges w/out repetition, and returns the cut cost
    int calculate_cost_and_get_list(ListDigraph&, ListDigraph::ArcMap<int>&, vector< vector <tuple <int, int, int>>>&, set<pair<int, int>>&);  

    // finds max flow for each terminal and stores all cut edges in multiway_cut
    void get_multiway_cut(ListDigraph&, ListDigraph::ArcMap<int>&, vector<int>&, vector<vector<tuple <int, int, int>>>&);
    
     // saves our result in file
    int save_result_in_file(string, set<pair<int, int>>&, int, double);

    void printing_multiway_vector(vector< vector <tuple <int, int, int> > >&);
    void printing_cut_list(set<pair<int, int>>&);

};

#endif

    