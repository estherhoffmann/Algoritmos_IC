/*
 * SimpleMultiwayDecoder.cpp
 */
#include <iostream>
#include <vector>
#include <algorithm>
#include <tuple>
#include <fstream>
#include <string>
#include <map>
#include "SimpleMultiwayDecoder.h"

using namespace std;

SimpleMultiwayDecoder::SimpleMultiwayDecoder(string file_name)
{
    cout << "leu alo?";
    read_file(file_name);
    cout << "leu arquivo";
}

SimpleMultiwayDecoder::~SimpleMultiwayDecoder() { }

double SimpleMultiwayDecoder::decode(const std::vector< double >& chromosome)
{
    // Valor do alelo indica em qual parte está o vértice (0->1/k, 1/k->2/k, ..., (k-1)/k->1)
    // Cromossomo possui, então, um alelo para cada vértice no grafo, menos pros terminais

    int cut_cost = 0;
    // Guarda as arestas do corte, como (u, v, peso)
    //set<pair<int, int>, int> cut_edges;
    vector<int> vertices_groups;
    vertices_groups.assign(num_of_v, -1);

    for(int i = 0; i < num_of_t; i++)
    {
        vertices_groups[terminals[i]] = i;
    }

    int vertex_index = 0;
    for(int i = 0; i < num_of_v; i++)
    {
        if (vertices_groups[i]==-1)
        {   // gambiarra pra nao atribuir ao vertice o grupo num_of_t+1
            // mas eh temporario, preciso descobrir como arrumar dpois
            if (chromosome[vertex_index] == 1)
                vertices_groups[i] = num_of_t-1;
            else
                vertices_groups[i] = (int)(chromosome[vertex_index]*(num_of_t));
            vertex_index++;
        }
    }

    map<pair<int, int>, int>::iterator itr;
    for (itr = edges.begin(); itr != edges.end(); ++itr)
    {
        if (vertices_groups[itr->first.first] != vertices_groups[itr->first.second])
        {
            cut_cost = cut_cost + itr->second;
        }
    }
	return (double)cut_cost;
}

// reads graph file
bool SimpleMultiwayDecoder::read_file(string file_name)
{
    int source, target, weight;
    cout << "hey";
    //specifying the full path
    string full_path = "../Multiway_cut/Instances/" + file_name;
    cout << "aquiii";
    // open a file in read mode
    ifstream graph_file;
    graph_file.open(full_path);

    if(graph_file.fail())
        return false;

    graph_file >> num_of_v;
    graph_file >> num_of_e;

    for(int i = 0; i < num_of_e; i++)
    {
        graph_file >> source; source--;
        graph_file >> target; target--;
        graph_file >> weight;
        edges.insert({make_pair(source, target), weight});
    }

    graph_file >> num_of_t;

    for(int i = 0; i < num_of_t; i++)
    {
        //reusing variable "source" to get the terminals from file
        graph_file >> source; source--;
        terminals.push_back(source);
    }

    graph_file.close();
    return true;
}


int SimpleMultiwayDecoder::get_num_of_v()
{
    return num_of_v;
}

int SimpleMultiwayDecoder::get_num_of_e()
{
    return num_of_e;
}

int SimpleMultiwayDecoder::get_num_of_t()
{
    return num_of_t;
}

vector<int> SimpleMultiwayDecoder::get_terminals()
{
    return terminals;
}

int SimpleMultiwayDecoder::get_edge_cost(std::pair<int, int> edge)
{
    auto search = edges.find(edge);
    if (search != edges.end())
    {
        return search->second;
    } else {
        cout << "Not found\n";
        return -1;
    }
}
