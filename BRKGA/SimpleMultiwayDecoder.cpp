/*
 * SimpleMultiwayDecoder.cpp
 */

#include "SimpleMultiwayDecoder.h"
#include <iostream>

using namespace std;

SimpleMultiwayDecoder::SimpleMultiwayDecoder() { }

SimpleMultiwayDecoder::~SimpleMultiwayDecoder() { }

double SimpleMultiwayDecoder::decode(const std::vector< double >& chromosome) const
{
    

	return 0;
}

// reads graph file
bool SimpleMultiwayDecoder::read_file(string file_name)
{
    int source, target, weight;

    //specifying the full path
    string full_path = "../Multiway_cut/Instances/" + file_name;

    // open a file in read mode
    ifstream graph_file;
    graph_file.open(full_path);

    if(graph_file.fail())
        return false;

    graph_file >> numof_v;
    graph_file >> numof_e;

    for(int i = 0; i < numof_e; i++)
    {
        graph_file >> source; source--;
        graph_file >> target; target--;
        graph_file >> weight;
        edges_cost.insert({make_pair(source, target), weight})
    }

    graph_file >> qnt_terminals;

    for(int i = 0; i < qnt_terminals; i++)
    {
        //reusing variable "source" to get the terminals from file
        graph_file >> source; source--;
        terminals.push_back(source);
    }

    graph_file.close();
    return true;
}


int get_num_of_v()
{
    return num_of_v;
}

int get_num_of_e()
{
    return num_of_e;
}

int get_qnt_terminals();
{
    return qnt_terminals;
}

vector get_terminals()
{
    return terminals;
}

int get_edge_cost(std::pair<int, int> edge)
{
    auto search = edges_cost.find(edge);
    if (search != edges_cost.end())
    {
        return search->second;
    } else {
        std::cout << "Not found\n";
        return -1;
    }
}
