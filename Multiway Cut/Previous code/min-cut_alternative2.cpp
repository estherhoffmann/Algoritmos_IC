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
int DEBUG_RESULT = false;

class Node
{
    public:
    //edge (u,v)
    int u;
    int v;
    int cost;
    Node* next;
};

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

void printing_multiway_vector(vector<vector<tuple <int,int,int> > > &multiway_cut)
{
    int num_edges = 0;
    cout << "Multiway_cut vector: " << endl;
    for(int i=0; i < multiway_cut.size(); i++)
    {
        cout << i << "º: ";
        for(int j=0; j < multiway_cut[i].size(); j++)
        {
            cout << "(" << get<0>(multiway_cut[i][j]) << ", " << get<1>(multiway_cut[i][j]) << ") ";
            cout << "cost: " << get<2>(multiway_cut[i][j]) << " ";
            num_edges++;
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
        cout << "(" << aux_q->u << ", " << aux_q->v << ", " << aux_q->cost << "),";
        aux_p = aux_q;
        aux_q = aux_p->next;
    }
    cout << "end of cut list" << endl;
}

int save_result_in_file(string file_name, Node *cut_list, int multiway_cut_cost, double exec_time)
{
    string full_path = "Solutions/Alternative 2/" + file_name.substr(0, file_name.find(".")) + ".sol";
    cout << full_path << endl;
    ofstream sol_file(full_path);

    sol_file << "custo " << multiway_cut_cost << endl;
    sol_file << "tempo " << fixed << setprecision(2) << exec_time << endl;

    Node *aux_p, *aux_q;
    aux_q = cut_list;
    while(aux_q != NULL)
    {
        sol_file << aux_q->u << " " << aux_q->v << " " << aux_q->cost << endl;
        aux_p = aux_q;
        aux_q = aux_p->next;
    }

    sol_file.close();
    return 0;
}

// reads graph file, creating a digraph
bool read_graph(ListDigraph &digraph, string file_name,
                ListDigraph::ArcMap<int> &capacity, vector<int> &terminals)
{
    int numof_e, numof_v, source, target, weight, qnt_terminals;
    ListDigraph::Arc arc;

    //specifying the full path
    string full_path = "Instances/" + file_name;

    // open a file in read mode
    ifstream graph_file;
    graph_file.open(full_path);

    if(graph_file.fail())
        return false;

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
    return true;
}

// searches new_edge in list. If found, does nothing
// if not found, insert in numerical order for  u atrr
bool insert_edge(Node *&list, tuple<int,int,int> &new_edge)
{
    Node *new_node = new Node();
    new_node->cost = get<2>(new_edge);

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
    {
        cout << "Edge already in the cut list. This wasn't supposed to happen :(.";
        return false;
    }


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

int calculate_cost_and_get_list(ListDigraph &digraph, ListDigraph::ArcMap<int> &capacity,
                                vector<vector<tuple <int,int,int> > > &multiway_cut,
                                Node*& cut_list, int &num_edges)
{
    int multiway_cut_cost = 0;
    num_edges = 0;

    for(int i=0; i < multiway_cut.size(); i++)
    {
        for(int j=0; j < multiway_cut[i].size(); j++)
        {
            if(insert_edge(cut_list, multiway_cut[i][j]))
            {
                multiway_cut_cost += get<2>(multiway_cut[i][j]);
                num_edges++;
            }
        }
    }
    return multiway_cut_cost;
}

//update the vector of Arcs and edges of the cut, using the mincut NodeMap
void update_multiwaycut(ListDigraph &digraph, ListDigraph::NodeMap<bool> &mincut, ListDigraph::ArcMap<int> &capacity,
                    vector<vector<tuple <int, int, int>>> &multiway_cut)
{
    ListDigraph::Node target_node;
    ListDigraph::Arc aux_arc;
    tuple <int, int, int> aux_tuple;
    vector<tuple<int, int, int>> aux_tuple_vector;

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
                    aux_arc = findArc(digraph, node_in_cut, target_node);
                    aux_tuple = make_tuple(digraph.id(node_in_cut)+1, digraph.id(target_node)+1, capacity[aux_arc]);
                    aux_tuple_vector.push_back(aux_tuple);
                    if (DEBUG >= 3)
                    {
                        cout << "(" << get<0>(aux_tuple)
                            << ", " << get<1>(aux_tuple)
                            << ", " << get<2>(aux_tuple) << ")" <<  endl;
                    }
                }
            }
        }
    }
    multiway_cut.push_back(aux_tuple_vector);
}


void get_multiway_cut(ListDigraph &digraph, ListDigraph::ArcMap<int> &capacity, vector<int> &terminals,
                   vector<vector<tuple <int, int, int>>> &multiway_cut)
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

       update_multiwaycut(digraph, mincut, capacity, multiway_cut);

       /* in this alternative, we delete from the graph the mincut of each iteration,
            so each terminal is already separate and there is no duplicated cut arcs */
       for(int arc_index = 0; arc_index < multiway_cut[current_term].size(); arc_index++)
       {
           digraph.erase(findArc(digraph, digraph.nodeFromId((get<0>(multiway_cut[current_term][arc_index]))-1),
                                           digraph.nodeFromId((get<1>(multiway_cut[current_term][arc_index])-1))));
       }

       if (DEBUG >= 2)
       {
           //printing_nodes_reachable_from_source_mincut(digraph, mincut);
           cout << endl;
           printing_mincut_values(min_cut_values);
           cout << endl;
       }

   }

   /* we also dont need to get rid of the expensivest cut because we will not compute a cut for the
        last terminal, since there will be no terminal connected in the graph */

   if (DEBUG >= 4)
   {
       cout << "-----" << endl << "Final print to make sure the graph has returned to original form: " << endl;
       printing_graph(digraph, capacity, terminals);
   }
}


int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cout << "This code requires a instance input. ";
        cout << "Please select one from the instances directory. (i.e. ./multiway1 steinb1.txt)";
        return 0;
    }

    auto start = chrono::steady_clock::now();

    ListDigraph digraph;
    ListDigraph::ArcMap<int> capacity(digraph);
    vector<int> terminals;

    string file_name = argv[1];
    if(!read_graph(digraph, file_name, capacity, terminals))
    {
        cout << "Could not open the file." << endl;
        return 0;
    }

    if (DEBUG >= 1)
    {
        printing_graph(digraph, capacity, terminals);
    }

    //vector of (vector of tuples) that storages the edges of the cut; this is a simpler result
    //(u, v, edge_cost)
    vector< vector <tuple <int, int, int> > > multiway_cut;
    get_multiway_cut(digraph, capacity, terminals, multiway_cut);

    Node* cut_list = NULL;
    int num_edges = 0;
    int multiway_cut_cost = calculate_cost_and_get_list(digraph, capacity, multiway_cut, cut_list, num_edges);

    if (DEBUG_RESULT == true)
    {
        cout << "---------" << endl << "Solution: " << endl;
        printing_multiway_vector(multiway_cut);
        printing_cut_list(cut_list);
        cout << "Number of edges: " << num_edges << endl;
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

    save_result_in_file(file_name, cut_list, multiway_cut_cost, time_taken);
    return 0;
}
