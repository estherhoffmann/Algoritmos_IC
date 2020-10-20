#include "MultiwaySolver.h"
#include "MultiwayDecoder.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace lemon;

MultiwayDecoder::MultiwayDecoder(string file_name) 
{ 
    file = file_name;

    //specifying the full path
    string full_path = "Instances/" + file;

    // open a file in read mode
    ifstream graph_file;
    graph_file.open(full_path);

    if(graph_file.fail())
    {
        num_of_v = -1;
        num_of_e = -1;
        num_of_t = -1;
    }

    graph_file >> num_of_v;
    graph_file >> num_of_e;

    for(int i=0; i <= num_of_e; i++)
    {
        graph_file.ignore( std::numeric_limits<std::streamsize>::max(), '\n');
    }

    graph_file >> num_of_t;
    graph_file.close();
}

MultiwayDecoder::~MultiwayDecoder() { }

double MultiwayDecoder::decode(const std::vector< double >& chromosome) const 
{   // aqui não estou usando ainda o cromossomo para alterar os custos das arestas,
    // então usar a brkga nisso por enquanto não adianta nada kkkk

    // instantiating Multiwaycut Solver object
    MultiwaySolver solver(0);

    // instantiating graph, arc map for edge costs, and vector of terminals
    ListDigraph digraph;
    ListDigraph::ArcMap<int> capacity(digraph);
    vector<int> terminals;

    if(!solver.read_file(digraph, file, capacity, terminals))
    {
        cout << "Could not open the file." << endl;
        return 0;
    }

    //vector of (vector of tuples) that storages the edges of the cut on each iteration
    vector< vector <tuple <int, int> > > multiway_cut;
    solver.get_multiway_cut(digraph, capacity, terminals, multiway_cut);

    set<pair<int, int>> cut_set;
    int multiway_cut_cost = 0;
    multiway_cut_cost = solver.calculate_cost_and_get_list(digraph, capacity, multiway_cut, cut_set);

	// sample fitness is the first allele
	return (double)multiway_cut_cost;
}

int MultiwayDecoder::get_num_of_v() { return num_of_v; }
int MultiwayDecoder::get_num_of_e() { return num_of_e; }
int MultiwayDecoder::get_num_of_t() { return num_of_t; }