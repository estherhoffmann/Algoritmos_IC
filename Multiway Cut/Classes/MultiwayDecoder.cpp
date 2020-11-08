#include "Multiway1Solver.h"
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
{   
    // instantiating Multiwaycut Solver object
    Multiway1Solver solver(0);

    // instantiating graph, arc map for edge costs, and vector of terminals
    ListDigraph digraph;
    ListDigraph::ArcMap<int> capacity(digraph);
    ListDigraph::ArcMap<int> perturbed_costs(digraph);
    vector<int> terminals;

    if(!solver.read_file(digraph, file, capacity, terminals))
    {
        cout << "Could not open the file." << endl;
        return 0;
    }

    /*
        Abaixo temos a manipulação dos custos das arestas de acordo com os cromossomos:
        Custo novo da aresta = (1 - alpha + 2 * alpha * alelo) * custo original

        sendo alpha um número real > 0 que determina a intensidade da perturbação.
        ~ Para um alelo < 0.5, o custo da aresta recebe um disconto;
            para um alelo > 0.5, o custo recebe ganho de valor
    */
   
    double perturb_intensity = 1; // alpha
    int chromosome_index = 0;

    for (ListDigraph::ArcIt arc(digraph); arc != INVALID; ++arc)
        perturbed_costs[arc] = -1;

    for (ListDigraph::ArcIt arc(digraph); arc != INVALID; ++arc)
    {
        if (perturbed_costs[arc] == -1)
        {
            perturbed_costs[arc] = ( 1 - perturb_intensity + ( 2 * perturb_intensity * chromosome[chromosome_index] ) ) * capacity[arc];

            ListDigraph::Arc reverse_arc = findArc(digraph, digraph.target(arc), digraph.source(arc));
            perturbed_costs[reverse_arc] = ( 1 - perturb_intensity + ( 2 * perturb_intensity * chromosome[chromosome_index] ) ) * capacity[reverse_arc];

            chromosome_index++;
        }
    }

    // vector of (vector of tuples) that storages the edges of the cut on each iteration
    vector< vector <tuple <int, int> > > multiway_cut;
    solver.get_multiway_cut(digraph, perturbed_costs, terminals, multiway_cut);

    set<pair<int, int>> cut_set;
    int multiway_cut_cost = solver.calculate_cost_and_get_list(digraph, capacity, multiway_cut, cut_set);

    return (double)multiway_cut_cost;
}

// faz a mesma coisa que a função decoder, mas nesta podemos extrair o cut_set tb. Precisamos dela justamente para 
// ter acesso às arestas do corte, já que o decoder precisa der uma estrutura definida.
void MultiwayDecoder::get_multiway_cut(const std::vector< double >& chromosome, int& multiway_cut_cost, set<pair<int, int>>& cut_set)
{
    // instantiating Multiwaycut Solver object
    Multiway1Solver solver(0);

    // instantiating graph, arc map for edge costs, and vector of terminals
    ListDigraph digraph;
    ListDigraph::ArcMap<int> capacity(digraph);
    ListDigraph::ArcMap<int> perturbed_costs(digraph);
    vector<int> terminals;

    if(!solver.read_file(digraph, file, capacity, terminals))
    {
        cout << "Could not open the file." << endl;
        return;
    }
   
    double perturb_intensity = 1; // alpha
    int chromosome_index = 0;

    for (ListDigraph::ArcIt arc(digraph); arc != INVALID; ++arc)
        perturbed_costs[arc] = -1;

    for (ListDigraph::ArcIt arc(digraph); arc != INVALID; ++arc)
    {
        if (perturbed_costs[arc] == -1)
        {
            perturbed_costs[arc] = ( 1 - perturb_intensity + ( 2 * perturb_intensity * chromosome[chromosome_index] ) ) * capacity[arc];
            ListDigraph::Arc reverse_arc = findArc(digraph, digraph.target(arc), digraph.source(arc));
            perturbed_costs[reverse_arc] = ( 1 - perturb_intensity + ( 2 * perturb_intensity * chromosome[chromosome_index] ) ) * capacity[reverse_arc];
            chromosome_index++;
        }
    }

    // vector of (vector of tuples) that storages the edges of the cut on each iteration
    vector< vector <tuple <int, int> > > multiway_cut;
    solver.get_multiway_cut(digraph, perturbed_costs, terminals, multiway_cut);

    multiway_cut_cost = solver.calculate_cost_and_get_list(digraph, capacity, multiway_cut, cut_set);
}

int MultiwayDecoder::get_num_of_v() const { return num_of_v; }
int MultiwayDecoder::get_num_of_e() const { return num_of_e; }
int MultiwayDecoder::get_num_of_t() const { return num_of_t; }