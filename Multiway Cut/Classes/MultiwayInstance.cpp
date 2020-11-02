#include "MultiwayInstance.h"

using namespace std;

MultiwayInstance::MultiwayInstance(string file) : capacity(digraph)
{
    file_name = file;
    read_file();
}

MultiwayInstance::~MultiwayInstance() { }

//read graph file and saves it in our graph structure
bool MultiwayInstance::read_file()
{
    int source, target, weight;
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

int MultiwayInstance::get_num_of_v() { return num_of_v; }
int MultiwayInstance::get_num_of_e() { return num_of_e; }
int MultiwayInstance::get_num_of_t() { return num_of_t; }

vector<int> MultiwayInstance::get_terminals() { return terminals; }

ListDigraph MultiwayInstance::get_digraph() { return digraph; }

ListDigraph::ArcMap<int> MultiwayInstance::get_capacity_arcmap() { return capacity; }

int MultiwayInstance::get_edge_cost(int u, int v)
{
    ListDigraph::Arc arc = findArc(digraph, u, v);
    return capacity[arc];
}