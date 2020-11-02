#ifndef MULTIWAYINSTANCE_H
#define MULTIWAYINSTANCE_H
#include <vector>
#include <string>
#include <set>
#include <fstream>

#include <lemon/list_graph.h>
#include <lemon/concepts/maps.h>
#include <lemon/concepts/digraph.h>

using namespace lemon;
using namespace std;

class MultiwayInstance {
private:
    string file_name;
    int num_of_v;
    int num_of_e;
    int num_of_t;

    ListDigraph digraph;
    ListDigraph::ArcMap<int> capacity;
    vector<int> terminals;

public:
	MultiwayInstance(string file);
	~MultiwayInstance();

    bool read_file();

    int get_num_of_v();
    int get_num_of_e();
    int get_num_of_t();
    vector<int> get_terminals();
    ListDigraph get_digraph();
    ListDigraph::ArcMap<int> get_capacity_arcmap();

    int get_edge_cost(int, int);

};

#endif
