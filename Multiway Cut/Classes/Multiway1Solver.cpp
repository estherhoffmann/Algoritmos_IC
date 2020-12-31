#include <iomanip>
#include <fstream>
#include "Multiway1Solver.h"
#include <lemon/preflow.h>

using namespace std;
using namespace lemon;

Multiway1Solver::Multiway1Solver(int debug)
{  
    DEBUG = debug;
}

Multiway1Solver::~Multiway1Solver() { }


void Multiway1Solver::printing_graph(ListDigraph &digraph, ListDigraph::ArcMap<int> &capacity, vector<int> &terminals)
{
    for (ListDigraph::ArcIt m(digraph); m != INVALID; ++m)
    {
        cout << "Arc: (" << digraph.id(digraph.source(m))+1
            << ", " << digraph.id(digraph.target(m))+1
            << "), cost: " << capacity[m]
            << ", id: " << digraph.id(m) << endl;
    }

    cout << endl << "Number of nodes: " << countNodes(digraph) << endl
         << "Number of arcs: " << countArcs(digraph) << endl
         << "Terminals:";
    for (auto it = terminals.begin(); it != terminals.end(); ++it)
        cout << ' ' << (*it)+1 ;
    cout << endl;
}

void Multiway1Solver::printing_multiway_vector(vector< vector <tuple <int, int> > > &multiway_cut)
{
    cout << "Multiway_cut vector: " << endl;
    for(int i=0; i < multiway_cut.size(); i++)
    {
        cout << i << "º: ";
        for(int j=0; j < multiway_cut[i].size(); j++)
        {
        cout << "(" << get<0>(multiway_cut[i][j]) << ", " << get<1>(multiway_cut[i][j]) << ") ";
        }
        cout << endl;
    }
    cout << endl;
}

void Multiway1Solver::printing_cut_list(set<pair<int, int>>& cut_list)
{
    cout << "Cut list:" << endl;

    // range-based for loop
    for (auto const &edge : cut_list) {
        cout << "(" << edge.first << ", "
                << edge.second << ")"
                << " ";
    }

    cout << "end of cut list" << endl;
}


// read graph file and saves it in our graph structure
bool Multiway1Solver::read_file(ListDigraph &digraph, string file_name,
                ListDigraph::ArcMap<int> &capacity, vector<int> &terminals)
{
    int num_of_e, num_of_v, num_of_t, source, target, weight;
    ListDigraph::Arc arc;

    //specifying the full path
    string full_path = "Instances/" + file_name;

    // open a file in read mode
    ifstream graph_file;
    graph_file.open(full_path);

    if(graph_file.fail())
        return false;

    graph_file >> num_of_v;
    graph_file >> num_of_e;

    for(int i = 0; i < num_of_v; i++)
        digraph.addNode();

    for(int i = 0; i < num_of_e; i++)
    {
        //source-- because in the file the node id starts with 1, and in lemon it starts with 0
        graph_file >> source; source--;
        graph_file >> target; target--;
        graph_file >> weight;

        arc = digraph.addArc(digraph.nodeFromId(source), digraph.nodeFromId(target));
        capacity[arc] = weight;
        arc = digraph.addArc(digraph.nodeFromId(target), digraph.nodeFromId(source));
        capacity[arc] = weight;
    }

    graph_file >> num_of_t;

    for(int i = 0; i < num_of_t; i++)
    {
        //reusing variable "source" to get the terminals from file
        graph_file >> source;
        terminals.push_back(source-1);
    }

    graph_file.close();
    return true;
}


// stores in cut_set the cut edges w/out repetition, and returns the cut cost
int Multiway1Solver::calculate_cost_and_get_list(ListDigraph &digraph, ListDigraph::ArcMap<int> &capacity,
                                vector<vector<tuple <int,int> > > &multiway_cut,
                                set<pair<int, int>>& cut_list)
{
    pair<int, int> aux_pair;
    int multiway_cut_cost = 0;

    for(int i=0; i < multiway_cut.size(); i++)
    {
        for(int j=0; j < multiway_cut[i].size(); j++)
        {
            if (get<0>(multiway_cut[i][j]) < get<1>(multiway_cut[i][j]))
                aux_pair = make_pair(get<0>(multiway_cut[i][j]), get<1>(multiway_cut[i][j]));
            else
                aux_pair = make_pair(get<1>(multiway_cut[i][j]), get<0>(multiway_cut[i][j]));
            
            if(get<1>(cut_list.insert(aux_pair)))
            {
                multiway_cut_cost += capacity[findArc(digraph, digraph.nodeFromId((get<0>(multiway_cut[i][j]))-1),
                                                digraph.nodeFromId((get<1>(multiway_cut[i][j])-1)) )];
            }
        }
    }
    return multiway_cut_cost;
}


int Multiway1Solver::position_highest_value_in_vector(vector<int> &vector)
{
    int position = 0, highest_value = 0;

    for(int i=0; i < vector.size(); i++)
    {
        if(vector[i] > highest_value)
        {
            highest_value = vector[i];
            position = i;
        }
    }
    return position;
}


//update the vector of Arcs and edges of the cut, using the mincut NodeMap
void Multiway1Solver::update_multiwaycut_and_arcs(ListDigraph &digraph, ListDigraph::NodeMap<bool> &mincut,
                    vector<vector<tuple <int, int>>> &multiway_cut)
{
    ListDigraph::Node target_node;
    tuple <int, int> aux_tuple;
    vector<tuple<int, int>> aux_tuple_vector;

    if (DEBUG >= 3)
        cout << "Arcs from mincut of this iteration:" << endl;

    for(ListDigraph::NodeIt node_in_cut(digraph); node_in_cut != INVALID; ++node_in_cut)
    {
        if(mincut[node_in_cut] == true)
        {
            for(ListDigraph::OutArcIt a(digraph, node_in_cut); a != INVALID; ++a)
            {
                target_node = digraph.target(a);
                if(mincut[target_node] == false)
                {
                    aux_tuple = make_tuple(digraph.id(node_in_cut)+1, digraph.id(target_node)+1);
                    aux_tuple_vector.push_back(aux_tuple);
                    if (DEBUG >= 3)
                    {
                        cout << "(" << get<0>(aux_tuple)
                            << ", " << get<1>(aux_tuple) << ")" <<  endl;
                    }
                }
            }
        }
    }
    multiway_cut.push_back(aux_tuple_vector);
}


// finds max flow for each terminal and stores all cut edges in multiway_cut
void Multiway1Solver::get_multiway_cut(ListDigraph &digraph, ListDigraph::ArcMap<int> &capacity, vector<int> &terminals,
                   vector<vector<tuple <int, int>>> &multiway_cut)
{
    // stores nodemap of maxflow algorithm
    ListDigraph::NodeMap<bool> mincut(digraph);
    
    // keeps the max flow value of each iteration
    vector<int> min_cut_values; 

    ListDigraph::Node artificial_sink = digraph.addNode();
    ListDigraph::Arc infinity_arc;

    //creating "infinity" arcs from all the terminals to the artificial_sink
    for (int t = 0; t < terminals.size(); t++)
    {
        infinity_arc = digraph.addArc(digraph.nodeFromId(terminals[t]), artificial_sink);
        capacity[infinity_arc] = numeric_limits<int>::max();
    }

    //computing mincut for each terminal and getting the arcs of the cut
    for (int current_term = 0; current_term < terminals.size(); current_term++)
    {
        digraph.erase(findArc(digraph, digraph.nodeFromId(terminals[current_term]), artificial_sink));

        if (DEBUG >= 2)
        {
            cout <<  "-----" << endl << "Iteration with source = terminal " << terminals[current_term] << endl << endl;
            if (DEBUG >=4)
            {
                printing_graph(digraph, capacity, terminals);
            }
        }

        //min-cut algorithm from lemon
        Preflow<ListDigraph> preflow(digraph, capacity, digraph.nodeFromId(terminals[current_term]), artificial_sink);
        preflow.runMinCut();
        preflow.minCutMap(mincut);
        min_cut_values.push_back(preflow.flowValue());

        update_multiwaycut_and_arcs(digraph, mincut, multiway_cut);

        infinity_arc = digraph.addArc(digraph.nodeFromId(terminals[current_term]), artificial_sink);
        capacity[infinity_arc] = numeric_limits<int>::max();
    }

    //deleting the infinity arcs and artificial_sink
    for (int t = 0; t < terminals.size(); t++)
    {
        digraph.erase(findArc(digraph, digraph.nodeFromId(terminals[t]), artificial_sink));
    }
    digraph.erase(artificial_sink);

    // getting rid of most expensive cut
    int most_exp_terminal_cut = position_highest_value_in_vector(min_cut_values);
    multiway_cut.erase(multiway_cut.begin()+most_exp_terminal_cut);

    if (DEBUG >= 4)
    {
        cout << "-----" << endl << "Final print to make sure the graph has returned to original form: " << endl;
        printing_graph(digraph, capacity, terminals);
    }
}


// saves our result in file
int Multiway1Solver::save_result_in_file(string file_name, set<pair<int, int>>& cut_list, int multiway_cut_cost, double exec_time)
{
    string full_path = "Solutions/Original algorithm/" + file_name.substr(0, file_name.find(".")) + ".sol";
    cout << full_path << endl;
    ofstream sol_file(full_path);

    sol_file << "custo " << multiway_cut_cost << endl;
    sol_file << "tempo " << fixed << setprecision(2) << exec_time << endl;

    for (auto const &edge : cut_list) {
        sol_file << edge.first << " " << edge.second << endl;
    }

    sol_file.close();
    return 0;
}