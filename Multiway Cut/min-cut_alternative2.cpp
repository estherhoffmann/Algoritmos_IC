#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <limits>
#include <iomanip>
//#include <time.h>

#include <lemon/list_graph.h>
#include <lemon/concepts/maps.h>
#include <lemon/concepts/digraph.h>
#include <lemon/preflow.h>

using namespace std;
using namespace lemon;

int DEBUG = 0;
int DEBUG_RESULT = false;

//these "printing_" functions are just to understand what is going on
void printing_graph(ListDigraph &digraph, ListDigraph::ArcMap<int> &capacity, vector<int> &terminals)
{
    for (ListDigraph::ArcIt m(digraph); m != INVALID; ++m)
    {
        cout << "Arc: (" << digraph.id(digraph.source(m))+1
            << ", " << digraph.id(digraph.target(m))+1
            << "), cost: " << capacity[m] << endl;
    }

    cout << endl << "Number of nodes: " << countNodes(digraph) << endl;
    cout << "Number of arcs: " << countArcs(digraph) << endl;
    cout << "Terminals:";
    for (auto it = terminals.begin(); it != terminals.end(); ++it)
        cout << ' ' << (*it)+1 ;
    cout << endl;
}


void printing_mincut_values(vector<int> &min_cut_values)
{
    cout << "Min Cut Values: ";
    for(int i=0; i < min_cut_values.size(); i++)
        cout << min_cut_values[i] << " ";
    cout << endl;
}


void printing_nodes_reachable_from_source_mincut(ListDigraph &digraph, ListDigraph::NodeMap<bool> &mincut)
{
    cout << "Nodes reachable from source in the mincut:";
    for (ListDigraph::NodeIt n(digraph); n != INVALID; ++n)
        if(mincut[n] == true)
            cout << " " << (digraph.id(n))+1;

    cout << endl;
}

void printing_multiway_vector(vector<vector<tuple <int,int> > > &multiway_cut)
{
    int num_edges = 0;
    cout << "Multiway_cut vector: " << endl;
    for(int i=0; i < multiway_cut.size(); i++)
    {
        cout << i << "º: ";
        for(int j=0; j < multiway_cut[i].size(); j++)
        {
            cout << "(" << get<0>(multiway_cut[i][j]) << ", " << get<1>(multiway_cut[i][j]) << ") ";
            num_edges++;
        }
        cout << endl;
    }
    cout << endl;
}


void printing_solution(ListDigraph &digraph,
                            vector<vector<tuple <int,int> > > &multiway_cut,
                            int num_edges, int multiway_cut_cost)
{
    printing_multiway_vector(multiway_cut);
    cout << "Number of edges: " << num_edges << endl;
    cout << "Cost of multiway cut: " << multiway_cut_cost << endl;
}

int save_result_in_file(string file_name, vector<vector<tuple <int,int> > > &multiway_cut,
                        int num_edges, int multiway_cut_cost)
{
    string full_path = "Solutions/" + file_name.substr(0, file_name.find(".")) + "_alternative2.sol";
    cout << full_path << endl;
    ofstream sol_file(full_path);

    sol_file << multiway_cut_cost << " " << num_edges << endl;

    int teste = 0;
    for(int i=0; i < multiway_cut.size(); i++)
    {
        for(int j=0; j < multiway_cut[i].size(); j++)
        {
            sol_file << get<0>(multiway_cut[i][j]) << " " << get<1>(multiway_cut[i][j]) << endl;
            teste++;
        }
    }
    sol_file.close();

    if (teste != num_edges)
        cout << "epa";
    return 0;
}

void read_graph(ListDigraph &digraph, string file_name,
                ListDigraph::ArcMap<int> &capacity, vector<int> &terminals)
{
    int numof_e, numof_v, source, target, weight, qnt_terminals;
    ListDigraph::Arc arc;

    //specifying the full path
    string full_path = "Instances/" + file_name;
    // open a file in read mode
    ifstream graph_file;
    graph_file.open(full_path);

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



//update the vector of Arcs and edges of the cut, using the mincut NodeMap
void update_multiwaycut_and_arcs(ListDigraph &digraph, ListDigraph::ArcMap<int> &capacity,
                    ListDigraph::NodeMap<bool> &mincut,
                    vector<vector<tuple <int, int>>> &multiway_cut, int &multiway_cut_cost, int &num_edges)
{
    ListDigraph::Node target_node;
    tuple <int, int> aux_tuple, reverse_aux_tuple;
    vector<tuple<int, int>> aux_tuple_vector;
    bool not_already_in_cut = true;

    if (DEBUG >= 3)
    {
        cout << "Arcs from mincut of this iteration:" << endl;
    }

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
                    aux_tuple = make_tuple(digraph.id(node_in_cut)+1, digraph.id(target_node)+1);
                    reverse_aux_tuple = make_tuple(get<1>(aux_tuple), get<0>(aux_tuple));

                    if (DEBUG >= 3)
                    {
                        cout << "(" << get<0>(aux_tuple)
                            << ", " << get<1>(aux_tuple) << ")" <<  endl;
                    }

                    //if tuple or reverse tuple are already in the multiway_cut
                    for(int i=0; i < multiway_cut.size(); i++)
                        if((find(multiway_cut[i].begin(), multiway_cut[i].end(), aux_tuple) != multiway_cut[i].end())
                            || (find(multiway_cut[i].begin(), multiway_cut[i].end(), reverse_aux_tuple) != multiway_cut[i].end()))
                            {
                                cout << "this should not have happened x.x";
                                not_already_in_cut = false;
                            }

                    if(not_already_in_cut)
                    {
                        aux_tuple_vector.push_back(aux_tuple);
                        multiway_cut_cost += capacity[findArc(digraph, digraph.nodeFromId((get<0>(aux_tuple)-1)),
                                                        digraph.nodeFromId((get<1>(aux_tuple)-1)) )];
                        digraph.erase(a);
                        num_edges++;
                    }

                }
            }
        }
    }
    multiway_cut.push_back(aux_tuple_vector);
}


int get_multiway_cut(ListDigraph &digraph, ListDigraph::ArcMap<int> &capacity, vector<int> &terminals,
                   vector<vector<tuple <int, int>>> &multiway_cut, int &num_edges)
{
   ListDigraph::NodeMap<bool> mincut(digraph);
   vector<int> min_cut_values; //keeps the max flow value of each iteration

   ListDigraph::Node artificial_sink = digraph.addNode();
   ListDigraph::Arc infinity_arc;
   int multiway_cut_cost = 0;

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
           cout <<  "-----" << endl << "Iteration with source = terminal " << terminals[current_term]+1 << endl << endl;
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

       update_multiwaycut_and_arcs(digraph, capacity, mincut, multiway_cut, multiway_cut_cost, num_edges);

       if (DEBUG >= 2)
       {
           //printing_nodes_reachable_from_source_mincut(digraph, mincut);
           cout << endl;
           printing_mincut_values(min_cut_values);
           cout << endl;
       }
       /*
       ListDigraph::Node curr_node = digraph.nodeFromId(terminals[current_term]);
       for(ListDigraph::OutArcIt a(digraph, curr_node); a != INVALID; ++a)
       {
           digraph.erase(a);
       }
       digraph.erase(curr_node);
       */
   }

   if (DEBUG >= 4)
   {
       cout << "-----" << endl << "Final print to make sure the graph has returned to original form: " << endl;
       printing_graph(digraph, capacity, terminals);
   }

   return multiway_cut_cost;
}


int main()
{
    time_t start, end;
    time(&start);
    ListDigraph digraph;
    ListDigraph::ArcMap<int> capacity(digraph);
    vector<int> terminals;

    string file_name;
    cin >> file_name;
    read_graph(digraph, file_name, capacity, terminals);

    if (DEBUG >= 1)
    {
        printing_graph(digraph, capacity, terminals);
    }

    //vector of (vector of tuples) that storages the edges of the cut; this is a simpler result
    vector< vector <tuple <int, int> > > multiway_cut;

    int num_edges = 0;
    int multiway_cut_cost = get_multiway_cut(digraph, capacity, terminals, multiway_cut, num_edges);

    if (DEBUG >= 1)
    {
        cout << endl << "Resulting graph: " << endl;
        printing_graph(digraph, capacity, terminals);
    }

    if (DEBUG_RESULT == true)
    {
        cout << "---------" << endl << "Solution: " << endl;
        printing_solution(digraph, multiway_cut, num_edges, multiway_cut_cost);
        cout << endl;
    }

    save_result_in_file(file_name, multiway_cut, num_edges, multiway_cut_cost);

    time(&end);
    // Calculating total time taken by the program.
    double time_taken = double(end - start);
    cout << "Time taken by program is : " << fixed
         << time_taken << setprecision(5);
    cout << " sec " << endl;
    return 0;
}
