#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <limits>

#include <lemon/list_graph.h>
#include <lemon/concepts/maps.h>
#include <lemon/concepts/digraph.h>
#include <lemon/preflow.h>



using namespace std;
using namespace lemon;

//these "printing_" functions are just to understand what is going on
void printing_graph_arcs(ListDigraph &digraph, ListDigraph::ArcMap<int> &capacity, vector<int> &terminals)
{
    for (ListDigraph::ArcIt m(digraph); m != INVALID; ++m)
    {
        cout << "Arc: (" << digraph.id(digraph.source(m))
            << ", " << digraph.id(digraph.target(m))
            << "), id: " << digraph.id(m)
            << ", cost: " << capacity[m] << endl;
    }

    cout << endl << "Number of nodes: " << countNodes(digraph) << endl;
    cout << "Number of arcs: " << countArcs(digraph) << endl;
    cout << "Terminals:";
    for (auto it = terminals.begin(); it != terminals.end(); ++it)
        cout << ' ' << *it ;
    cout << endl << endl;
}


void printing_minimum_cut(ListDigraph &digraph, vector<int> &terminals,
                            vector<vector<ListDigraph::Arc>> &cut_arcs, vector<int> &max_flow_values)
{
    cout << "Min cut values: ";
    for(int i=0; i < max_flow_values.size(); i++)
        cout << max_flow_values[i] << " ";

    cout << endl << "Arcs of the (total) cut:";
    for (int terminal=0; terminal < cut_arcs.size(); terminal++)
        for(int arc=0; arc < cut_arcs[terminal].size(); arc++)
            cout << " " << digraph.id(cut_arcs[terminal][arc]);
    cout << endl << endl;
}


void printing_nodes_reachable_from_source_mincut(ListDigraph &digraph, ListDigraph::NodeMap<bool> &mincut)
{
    cout << "Nodes reachable from source in the mincut:";
    for (ListDigraph::NodeIt n(digraph); n != INVALID; ++n)
        if(mincut[n] == true)
            cout << " " << digraph.id(n);

    cout << endl;
}


//reads graph file, creating a digraph
void read_graph(ListDigraph &digraph, string file_name,
                ListDigraph::ArcMap<int> &capacity, vector<int> &terminals)
{
    int numof_e, numof_v, source, target, weight, qnt_terminals;
    ListDigraph::Arc arc;

    // open a file in read mode
    ifstream graph_file;
    graph_file.open(file_name);

    graph_file >> numof_v;
    graph_file >> numof_e;

    for(int i = 0; i < numof_v; i++)
        digraph.addNode();


    for(int i = 0; i < numof_e; i++)
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

    graph_file >> qnt_terminals;

    for(int i = 0; i < qnt_terminals; i++)
    {
        //reusing variable "source" to get the terminals from file
        graph_file >> source;
        terminals.push_back(source-1);
    }

    graph_file.close();
}


int position_highest_value_in_vector(vector<int> &vector)
{
    int position, highest_value = 0;

    for(int i=0; i < vector.size(); i++)
    {
        if(vector[i] > highest_value);
        {
            highest_value = vector[i];
            position = i;
        }
    }
    return position;
}


int get_rid_of_most_expensive_cut(ListDigraph &digraph, ListDigraph::ArcMap<int> &capacity,
                                vector<vector<ListDigraph::Arc> > &cut_arcs,
                                vector<vector<tuple <int,int> > > &multiway_cut,
                                vector<int> &max_flow_values)
{
    int multiway_cut_cost = 0;
    int most_exp_terminal_cut = position_highest_value_in_vector(max_flow_values);
    int edge_cost = 0;

    cut_arcs.erase(cut_arcs.begin()+most_exp_terminal_cut);
    multiway_cut.erase(multiway_cut.begin()+most_exp_terminal_cut);

    //cout << endl << "Adicionando: " << endl;
    for(int i=0; i < multiway_cut.size(); i++)
    {
        for(int j=0; j < multiway_cut[i].size(); j++)
        {
            edge_cost = capacity[findArc(digraph, digraph.nodeFromId((get<0>(multiway_cut[i][j]))-1),
                                            digraph.nodeFromId((get<1>(multiway_cut[i][j])-1)) )];
            //cout << "(" << get<0>(multiway_cut[i][j]) << ", "
            //            << get<1>(multiway_cut[i][j]) << ") = "<< edge_cost << endl;
            multiway_cut_cost += edge_cost;
        }
    }
    //cout << endl << endl;
    return multiway_cut_cost;
}


//update the vector of Arcs and edges of the cut, using the mincut NodeMap
void update_multiwaycut_and_arcs(ListDigraph &digraph, ListDigraph::NodeMap<bool> &mincut,
                    vector<vector<ListDigraph::Arc>> &cut_arcs, vector<vector<tuple <int, int>>> &multiway_cut)
{
    ListDigraph::Node target_node;
    tuple <int, int> aux_tuple, reverse_aux_tuple;
    vector<ListDigraph::Arc> aux_arcs_vector;
    vector<tuple<int, int>> aux_tuple_vector;
    bool not_already_in_cut;

    //cout << "-" << endl << "Arcs from mincut of this iteration:" << endl;
    for(ListDigraph::NodeIt node_in_cut(digraph); node_in_cut != INVALID; ++node_in_cut)
    {
        if(mincut[node_in_cut] == true)
        {
            for(ListDigraph::OutArcIt a(digraph, node_in_cut); a != INVALID; ++a)
            {
                not_already_in_cut = true;
                target_node = digraph.target(a);
                if(mincut[target_node] == false)
                {
                    aux_arcs_vector.push_back(a);

                    aux_tuple = make_tuple(digraph.id(node_in_cut)+1, digraph.id(target_node)+1);
                    reverse_aux_tuple = make_tuple(get<1>(aux_tuple), get<0>(aux_tuple));

                    //cout << " " << digraph.id(a) << ": (" << get<0>(aux_tuple)
                    //    << ", " << get<1>(aux_tuple) << ")" <<  endl;

                    //if tuple or reverse tuple are already in the multiway_cut
                    for(int i=0; i < multiway_cut.size(); i++)
                        if((find(multiway_cut[i].begin(), multiway_cut[i].end(), aux_tuple) != multiway_cut[i].end())
                            || (find(multiway_cut[i].begin(), multiway_cut[i].end(), reverse_aux_tuple) != multiway_cut[i].end()))
                                not_already_in_cut = false;

                    if(not_already_in_cut)
                    {
                        aux_tuple_vector.push_back(aux_tuple);
                    }

                }
            }
        }
    }

    cut_arcs.push_back(aux_arcs_vector);
    multiway_cut.push_back(aux_tuple_vector);
}

//get cut_arcs, multiway_cut and returns the cost of this cut, based on our digraph and terminals
int get_multiway_cut(ListDigraph &digraph, ListDigraph::ArcMap<int> &capacity, vector<int> &terminals,
                   vector<vector<ListDigraph::Arc>> &cut_arcs, vector<vector<tuple <int, int>>> &multiway_cut)
{
   ListDigraph::NodeMap<bool> mincut(digraph);
   vector<int> max_flow_values; //keeps the max flow value of each iteration

   ListDigraph::Node artificial_sink = digraph.addNode();
   ListDigraph::Arc infinity_arc;

   //creating infinity arcs from all the terminals to the artificial_sink
   for (int t = 0; t < terminals.size(); t++)
   {
       infinity_arc = digraph.addArc(digraph.nodeFromId(terminals[t]), artificial_sink);
       capacity[infinity_arc] = numeric_limits<int>::max();
   }

   //computing mincut for each terminal and getting the arcs of the cut
   for (int current_term = 0; current_term < terminals.size(); current_term++)
   {
       digraph.erase(findArc(digraph, digraph.nodeFromId(terminals[current_term]), artificial_sink));

       //cout <<  "-----" << endl << "Iteration with source = terminal " << terminals[current_term] << endl << endl;
       //printing_graph_arcs(digraph, capacity, terminals);

       //min-cut algorithm from lemon
       Preflow<ListDigraph> preflow(digraph, capacity, digraph.nodeFromId(terminals[current_term]), artificial_sink);
       preflow.runMinCut();
       preflow.minCutMap(mincut);
       max_flow_values.push_back(preflow.flowValue());

       update_multiwaycut_and_arcs(digraph, mincut, cut_arcs, multiway_cut);

       //printing_nodes_reachable_from_source_mincut(digraph, mincut);
       //printing_minimum_cut(digraph, terminals, cut_arcs, max_flow_values);

       infinity_arc = digraph.addArc(digraph.nodeFromId(terminals[current_term]), artificial_sink);
       capacity[infinity_arc] = numeric_limits<int>::max();
   }

   //deleting the infinity arcs and artificial_sink
   for (int t = 0; t < terminals.size(); t++)
   {
       digraph.erase(findArc(digraph, digraph.nodeFromId(terminals[t]), artificial_sink));
   }
   digraph.erase(artificial_sink);

   int multiway_cut_cost = get_rid_of_most_expensive_cut(digraph, capacity, cut_arcs, multiway_cut, max_flow_values);

   //cout << "-----" << endl << "Final print to make sure the graph has returned to original form: " << endl;
   //printing_graph_arcs(digraph, capacity, terminals);

   return multiway_cut_cost;
}

int main()
{
    ListDigraph digraph;
    ListDigraph::ArcMap<int> capacity(digraph);
    vector<int> terminals;

    string file_name;
    cin >> file_name;
    read_graph(digraph, file_name, capacity, terminals);
    //printing_graph_arcs(digraph, capacity, terminals);

    //vector of vectors that storages the Arcs of the cut of each iteration.
    vector< vector<ListDigraph::Arc> > cut_arcs;
    //vector of (vector of tuples) that storages the edges of the cut; this is a simpler result
    vector< vector <tuple <int, int> > > multiway_cut;

    int multiway_cut_cost = get_multiway_cut(digraph, capacity, terminals, cut_arcs, multiway_cut);

    cout << "Arcs of the multiway cut: ";
    int num_arcs = 0;
    for (int terminal=0; terminal < cut_arcs.size(); terminal++)
        for(int arc=0; arc < cut_arcs[terminal].size(); arc++)
        {
            cout << digraph.id(cut_arcs[terminal][arc]) << " ";
            num_arcs++;
        }
    cout << endl << "Number of arcs:" << num_arcs << endl << endl;

    cout << "Multiway_cut: ";
    int num_edges = 0;
    for (int terminal=0; terminal < multiway_cut.size(); terminal++)
        for(int tuple=0; tuple < multiway_cut[terminal].size(); tuple++)
        {
            cout << "(" << get<0>(multiway_cut[terminal][tuple]) << ", " << get<1>(multiway_cut[terminal][tuple]) << ") ";
            num_edges++;
        }
        cout << endl << "Number of edges:" << num_edges << endl << endl;

    cout << "Cost of multiway cut: " << multiway_cut_cost << endl;

    return 0;
}
