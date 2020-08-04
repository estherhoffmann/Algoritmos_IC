#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <tuple>
#include <limits>
#include <iomanip>
#include <chrono>

#include <lemon/list_graph.h>
#include <lemon/concepts/maps.h>
#include <lemon/concepts/digraph.h>
#include <lemon/preflow.h>

using namespace std;
using namespace lemon;

int DEBUG = 0;
int DEBUG_EXPENSIVE_CUT = false;
int DEBUG_RESULT = true;

// link list node
class Node
{
    public:
    //edge (u,v)
    int u;
    int v;
    Node* next;
};

// these "printing_" functions are just to understand what is going on
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

void printing_cut_list(Node*& list)
{
    Node *aux_p, *aux_q;
    cout << "Cut list:" << endl;
    aux_q = list;
    while(aux_q != NULL)
    {
        cout << "(" << aux_q->u << ", " << aux_q->v << ")" << endl;
        aux_p = aux_q;
        aux_q = aux_p->next;
    }
        cout << "End of cut list" << endl;
}

void printing_solution(ListDigraph &digraph,
                            vector<vector<tuple <int,int> > > &multiway_cut,
                            Node* cut_list,
                            int num_edges, int multiway_cut_cost)
{
    printing_multiway_vector(multiway_cut);
    printing_cut_list(cut_list);
    cout << "Number of edges: " << num_edges << endl;
    cout << "Cost of multiway cut: " << multiway_cut_cost << endl;
}

int save_result_in_file(string file_name, Node *cut_list, int multiway_cut_cost, double exec_time)
{
    string full_path = "Solutions/" + file_name.substr(0, file_name.find(".")) + ".sol";
    cout << full_path << endl;
    ofstream sol_file(full_path);

    sol_file << "custo " << multiway_cut_cost << endl;
    sol_file << "tempo " << fixed << setprecision(2) << exec_time << endl;

    Node *aux_p, *aux_q;
    aux_q = cut_list;
    while(aux_q != NULL)
    {
        sol_file << aux_q->u << " " << aux_q->v << endl;
        aux_p = aux_q;
        aux_q = aux_p->next;
    }

    sol_file.close();
    return 0;
}

// reads graph file, creating a digraph
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

// searches new_edge in list. If found, does nothing
// if not found, insert in numerical order for  u atrr
bool insert_edge(Node *&list, tuple<int,int> &new_edge)
{
    Node *new_node = new Node();
    //ordening new node by attribute u
    if (get<0>(new_edge) < get<1>(new_edge))
    {
        new_node->u = get<0>(new_edge);
        new_node->v = get<1>(new_edge);
    }
    else
    {
        new_node->u = get<1>(new_edge);
        new_node->v = get<0>(new_edge);
    }

    // if list is empty
    if (list == nullptr)
    {
        new_node->next = NULL;
        list = new_node;
        return true;
    }

    // else
    Node *aux_curr, *aux_prev;
    aux_curr = list;
    aux_prev = nullptr;

    while((aux_curr != nullptr) && ((aux_curr->u < new_node->u) || ((aux_curr->u == new_node->u) && (aux_curr->v < new_node->v))))
    {
        aux_prev = aux_curr;
        aux_curr = aux_prev->next;
    }

    //edge already in the list
    if(aux_curr != nullptr && aux_curr->u == new_node->u && aux_curr->v == new_node->v)
        return false;

    //we want to insert in the fist position
    if(aux_prev == nullptr)
    {
        new_node->next = aux_curr;
        list = new_node;
        return true;
    }

    //we want to insert or between 2 nodes, or at the end
    new_node->next = aux_curr;
    aux_prev->next = new_node;
    return true;
}

int position_highest_value_in_vector(vector<int> &vector)
{
    int position = 0, highest_value = 0;

    for(int i=0; i < vector.size(); i++)
    {
        if(vector[i] > highest_value)
        {
            //cout << "position: " << position << ", highest_value: " << highest_value << endl;
            highest_value = vector[i];
            position = i;
        }
    }
    return position;
}

int calculate_cost_and_get_list(ListDigraph &digraph, ListDigraph::ArcMap<int> &capacity,
                                vector<vector<tuple <int,int> > > &multiway_cut,
                                Node*& cut_list, int &num_edges)
{
    int edge_cost, multiway_cut_cost = 0;
    tuple <int, int> aux_tuple;
    num_edges = 0;

    for(int i=0; i < multiway_cut.size(); i++)
    {
        for(int j=0; j < multiway_cut[i].size(); j++)
        {
            aux_tuple = multiway_cut[i][j];
            if(insert_edge(cut_list, aux_tuple))
            {
                edge_cost = capacity[findArc(digraph, digraph.nodeFromId((get<0>(multiway_cut[i][j]))-1),
                                                digraph.nodeFromId((get<1>(multiway_cut[i][j])-1)) )];
                multiway_cut_cost += edge_cost;
                num_edges++;
            }
        }
    }
    return multiway_cut_cost;
}

void get_rid_of_most_expensive_cut(ListDigraph &digraph,
                                vector<vector<tuple <int,int> > > &multiway_cut,
                                vector<int> &min_cut_values)
{
    int most_exp_terminal_cut = position_highest_value_in_vector(min_cut_values);

    if (DEBUG_EXPENSIVE_CUT == 1)
    {
        cout << "Getting rid of most expensive cut...." << endl;
        printing_multiway_vector(multiway_cut);
        printing_mincut_values(min_cut_values);
        cout << "Most expensive terminal cut: " << most_exp_terminal_cut
            << " with value " << min_cut_values[most_exp_terminal_cut]<< endl;
    }

    multiway_cut.erase(multiway_cut.begin()+most_exp_terminal_cut);

    if (DEBUG_EXPENSIVE_CUT == 1)
        printing_multiway_vector(multiway_cut);
}

//update the vector of Arcs and edges of the cut, using the mincut NodeMap
void update_multiwaycut_and_arcs(ListDigraph &digraph, ListDigraph::NodeMap<bool> &mincut,
                    vector<vector<tuple <int, int>>> &multiway_cut)
{
    ListDigraph::Node target_node;
    tuple <int, int> aux_tuple;
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


void get_multiway_cut(ListDigraph &digraph, ListDigraph::ArcMap<int> &capacity, vector<int> &terminals,
                   vector<vector<tuple <int, int>>> &multiway_cut)
{
   ListDigraph::NodeMap<bool> mincut(digraph);
   vector<int> min_cut_values; //keeps the max flow value of each iteration

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

       if (DEBUG >= 2)
       {
           //printing_nodes_reachable_from_source_mincut(digraph, mincut);
           cout << endl;
           printing_mincut_values(min_cut_values);
           cout << endl;
       }

       infinity_arc = digraph.addArc(digraph.nodeFromId(terminals[current_term]), artificial_sink);
       capacity[infinity_arc] = numeric_limits<int>::max();
   }

   //deleting the infinity arcs and artificial_sink
   for (int t = 0; t < terminals.size(); t++)
   {
       digraph.erase(findArc(digraph, digraph.nodeFromId(terminals[t]), artificial_sink));
   }
   digraph.erase(artificial_sink);

   get_rid_of_most_expensive_cut(digraph, multiway_cut, min_cut_values);

   if (DEBUG >= 4)
   {
       cout << "-----" << endl << "Final print to make sure the graph has returned to original form: " << endl;
       printing_graph(digraph, capacity, terminals);
   }
}


int main()
{
    auto start = chrono::steady_clock::now();
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
    get_multiway_cut(digraph, capacity, terminals, multiway_cut);

    Node* cut_list = NULL;
    int num_edges = 0;
    int multiway_cut_cost = calculate_cost_and_get_list(digraph, capacity, multiway_cut, cut_list, num_edges);
    //printing_cut_list(cut_list);

    if (DEBUG_RESULT == true)
    {
        cout << "---------" << endl << "Solution: " << endl;
        printing_solution(digraph, multiway_cut, cut_list, num_edges, multiway_cut_cost);
        cout << endl;
    }

    // Calculating total time taken by the program.
    auto end = chrono::steady_clock::now();
    chrono::duration<double> diff = end - start;
    double time_taken = diff.count();
    cout << "Time taken by program is : " << fixed
         << fixed << setprecision(2) << time_taken;
    cout << " sec " << endl;

    save_result_in_file(file_name, cut_list, multiway_cut_cost, time_taken);
    return 0;
}
