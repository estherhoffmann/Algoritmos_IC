#ifndef MULTIWAYSORTSOLVER_H
#define MULTIWAYSORTSOLVER_H
#include <iostream>
#include <vector>
#include <string>
#include <set>

#include <lemon/list_graph.h>
#include <lemon/concepts/maps.h>
#include <lemon/concepts/digraph.h>

using namespace lemon;
using namespace std;

class MultiwaySortSolver {
private:
    // variable for debugging
    int DEBUG;

    // functions that can be only used inside our public functions
    int position_highest_value_in_vector(vector<int>&); 
    void update_multiwaycut_and_arcs(ListDigraph&, ListDigraph::NodeMap<bool>&, ListDigraph::ArcMap<int>&, vector<vector<tuple <int, int, int>>>&);

    vector<int> get_terminal_order(vector<int>);

    // runs original multiway cut algorithm and returns vector with desirable terminal order
    vector<int> get_expensivest_cut(ListDigraph &, ListDigraph::ArcMap<int> &, ListDigraph::NodeMap<bool> &,
                    vector<int> &, vector<vector<tuple <int, int, int>>> &);


public:

	MultiwaySortSolver(int);
	~MultiwaySortSolver();

    void printing_graph(ListDigraph&, ListDigraph::ArcMap<int>&, vector<int>&);
    void printing_multiway_vector(vector< vector <tuple <int, int, int> > >&);
    void printing_cut_list(set<tuple<int, int, int>>&);
       
    // read graph file and saves it in our graph structure
    bool read_file(ListDigraph&, string, ListDigraph::ArcMap<int>&, vector<int>&);

    // stores in cut_set the cut edges w/out repetition, and returns the cut cost
    int calculate_cost_and_get_list(vector<vector<tuple <int,int, int> > > &, set<tuple<int, int, int>>& );  

    // finds max flow for each terminal and stores all cut edges in multiway_cut
    void get_multiway_cut(ListDigraph&, ListDigraph::ArcMap<int>&, vector<int>&, vector< vector <tuple <int, int, int>>>&);
    
     // saves our result in file
    int save_result_in_file(string, set<tuple<int, int, int>>&, int, double);

    

};

#endif

    