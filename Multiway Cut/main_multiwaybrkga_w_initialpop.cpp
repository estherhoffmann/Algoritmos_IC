#include <iostream>
#include <chrono>
#include <iomanip>
#include <stdlib.h>
#include <fstream>

#include "Classes/SimpleMultiwayDecoder.h"
#include "Classes/MultiwayDecoder.h"
#include "BRKGA/MTRand.h"
#include "BRKGA/BRKGA.h"

#include "Classes/MultiwayAlternativeSolver.h"
#include "Classes/MultiwaySortSolver.h"

using namespace std;

int DEBUG = 0;
int DEBUG_RESULT = false;

void decoder_test(string file_name, const vector< double >& chromosome)
{
    MultiwayDecoder decoder(file_name);	
    int cost = decoder.decode(chromosome);
    cout << "Cost: " << cost << endl;
}

// Arestas do corte recebem 0.9 no cromossomo. Demais arestas recebem 0.1
void turn_into_chromosome(ListDigraph &digraph, ListDigraph::ArcMap<int> &capacity,
                        set<tuple<int, int, int>> &cut_set, vector<double> &aux_chromosome, bool ch_DEBUG=false)
{

    // Como o vetor de chromossomo é usado na ordem que o lemon itera sobre as arestas,
    // vamos iterar sob todas as arestas para pegar as posições dessas no chromossomo
    // e alterar o valor do alelo nas arestas que estão no corte
    tuple<int, int, int> aux_tuple, aux_reverse_tuple;
    ListDigraph::ArcMap<bool> visited(digraph);
    ListDigraph::Arc reverse_arc;
    int chromosome_index = 0;

    // marcando todos arcos como não visitados
    for(ListDigraph::ArcIt arc(digraph); arc != INVALID; ++arc)
        visited[arc] = false;

    // limpando o chromossomo só por via das dúvidas :v
    for(int i=0; i < aux_chromosome.size(); i++)
        aux_chromosome[i] = -1;

    if (ch_DEBUG == true)
    {
        // printar chromossomo para conferir
        cout << "-----" << endl << "Cromossomo: ";
        for(int i = 0; i < aux_chromosome.size(); i++)
            cout << aux_chromosome[i] << " ";
        cout << endl;
    }
    
    for(ListDigraph::ArcIt arc(digraph); arc != INVALID; ++arc)
    {   
        if (ch_DEBUG == true)
            cout << "Arco: (" << digraph.id(digraph.source(arc))+1 << ", " << digraph.id(digraph.target(arc))+1 << "). ";

        if(visited[arc] == false)
        {
            if (ch_DEBUG == true)
                cout << "Ainda não foi visitado. ";

            aux_tuple = make_tuple(digraph.id(digraph.source(arc))+1, digraph.id(digraph.target(arc))+1, capacity[arc]);
            aux_reverse_tuple = make_tuple(digraph.id(digraph.target(arc))+1, digraph.id(digraph.source(arc))+1, capacity[arc]);

            if ((cut_set.find(aux_tuple) != cut_set.end()) || (cut_set.find(aux_reverse_tuple) != cut_set.end()))
            {
                if (ch_DEBUG == true)
                    cout << "Está no corte. ";
                aux_chromosome[chromosome_index] = 0.9;
            }
            else
            {
                if (ch_DEBUG == true)
                    cout << "Não está no corte. ";
                aux_chromosome[chromosome_index] = 0.1;
            }
            
            if (ch_DEBUG == true)
                cout << "Arco marcado como visitado. ";
            visited[arc] = true;
            reverse_arc = findArc(digraph, digraph.target(arc), digraph.source(arc));
            visited[reverse_arc] = true;

            chromosome_index++;
        }
        else
            if (ch_DEBUG == true)
                cout << "Já foi visitado. ";

        if (ch_DEBUG == true)
            cout << endl;
    }
}

set<tuple<int,int,int>> run_multiwaysort(string file_name)
{

    ListDigraph digraph;
    ListDigraph::ArcMap<int> capacity(digraph);
    vector<int> terminals;
    set<tuple<int, int, int>> cut_set;

    // instantiating Multiwaycut Sort Solver object
    MultiwaySortSolver multiwaysort(DEBUG);
    
    if(!multiwaysort.read_file(digraph, file_name, capacity, terminals))
    {
        cout << "Could not open the file." << endl;
        return cut_set;
    }

    if (DEBUG >= 1)
    {
        multiwaysort.printing_graph(digraph, capacity, terminals);
    }

    //vector of (vector of tuples) that storages the edges of the cut on each iteration
    vector< vector <tuple <int, int, int> > > multiway_cut;

    multiwaysort.get_multiway_cut(digraph, capacity, terminals, multiway_cut);
    int multiway_cut_cost = multiwaysort.calculate_cost_and_get_list(multiway_cut, cut_set);

    if (DEBUG_RESULT == true)
    {
        cout << "---------" << endl << "Solution: " << endl;
        multiwaysort.printing_multiway_vector(multiway_cut);
        multiwaysort.printing_cut_list(cut_set);
        cout << "Cost of multiway cut: " << multiway_cut_cost << endl;
        cout << endl;
    }

    return cut_set;
}

set<tuple<int, int, int>> run_multiway3shuffled (string file_name, int &cut_cost)
{
    ListDigraph digraph;
    ListDigraph::ArcMap<int> capacity(digraph);
    vector<int> terminals;
    set<tuple<int, int, int>> cut_set;

    // instantiating Multiwaycut Alternative (multiway3) Solver object 
    MultiwayAlternativeSolver solver(DEBUG);

    if(!solver.read_file(digraph, file_name, capacity, terminals))
    {
        cout << "Could not open the file." << endl;
        return cut_set;
    }

    if (DEBUG >= 1)
    {
        solver.printing_graph(digraph, capacity, terminals);
    }

    //vector of (vector of tuples) that storages the edges of the cut on each iteration
    vector< vector <tuple <int, int, int> > > multiway_cut;

    // vector terminals_order determinates the order in which the algorithm will calculate min cuts.
    // the first min cut will be calculated for terminals[terminals_order[0]] vertex
    // so, for all x in terminals_order, 0 < x < terminals.size() 
    vector<int> terminals_order;      
    
    // shuffle
    for (int i = 0; i < terminals.size(); i++)
        terminals_order.push_back(i);

    srand(time(NULL));  

    for (int i = terminals.size() - 1; i > 0; i--)  
    {  
        // Pick a random index from 0 to i  
        int j = rand() % (i + 1);  

        // Swap arr[i] with the element at random index  
        swap(terminals_order[i], terminals_order[j]);  
    }  

    solver.get_multiway_cut(digraph, capacity, terminals, terminals_order, multiway_cut);

    cut_cost = solver.calculate_cost_and_get_list(digraph, capacity, multiway_cut, cut_set);

    return cut_set;
}

void save_result(string file_name, string path, set<pair<int, int>> cut_list, int multiway_cut_cost, double exec_time)
{
    string full_path = "Solutions/" + path + file_name.substr(0, file_name.find(".")) + ".sol";
    cout << full_path << endl;
    ofstream sol_file(full_path);

    sol_file << "custo " << multiway_cut_cost << endl;
    sol_file << "tempo " << fixed << setprecision(2) << exec_time << endl;

    for (auto const &edge : cut_list) {
        sol_file << edge.first << " " << edge.second << endl;
    }

    sol_file.close();
}

// run BRKGA with simple solver for multiway cut
void run_simple_BRKGA(string file_name, int& cost, set<pair<int, int>>& cut_set, vector<vector<double>> ini_population)
{
    SimpleMultiwayDecoder decoder(file_name);			// initialize the decoder

    int num_of_v = decoder.get_num_of_v();
    int num_of_t = decoder.get_num_of_t();

    const unsigned n = (num_of_v - num_of_t);    // size of chromosomes
	const unsigned p = 1000;	                    // size of population
	const double pe = 0.20;		                    // fraction of population to be the elite-set
	const double pm = 0.10;		                    // fraction of population to be replaced by mutants
	const double rhoe = 0.70;	                    // probability that offspring inherit an allele from elite parent
	const unsigned K = 3;		                    // number of independent populations 
	const unsigned MAXT = 1;	                    // number of threads for parallel decoding

	const long unsigned rngSeed = 0;	// seed to the random number generator
	MTRand rng(rngSeed);				// initialize the random number generator

    // initialize the BRKGA-based heuristic
	BRKGA< SimpleMultiwayDecoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);
   
    unsigned generation = 0;		// current generation
	const unsigned X_INTVL = 100;	// exchange best individuals at every 100 generations
	const unsigned X_NUMBER = 2;	// exchange top 2 best
	const unsigned MAX_GENS = 1000;	// run for 1000 gens

	do {
		algorithm.evolve();	// evolve the population for one generation

		if((++generation) % X_INTVL == 0) {
			algorithm.exchangeElite(X_NUMBER);	// exchange top individuals
		}
	} while (generation < MAX_GENS);

    cost = (int)algorithm.getBestFitness();
    vector<double> solution = algorithm.getBestChromosome();
    cut_set = decoder.get_cut_list(solution);
}

// run BRKGA with our approximation algorithm multiway1
void run_BRKGA(string file_name, int& cost, set<pair<int, int>> &cut_set, vector<vector<double>> &ini_population)
{
    MultiwayDecoder decoder(file_name);			// initialize the decoder
    int num_of_e = decoder.get_num_of_e();

    const unsigned n = num_of_e;	                // size of chromosomes
	const unsigned p = 100;	                        // size of population
	const double pe = 0.20;		                    // fraction of population to be the elite-set
	const double pm = 0.10;		                    // fraction of population to be replaced by mutants
	const double rhoe = 0.70;	                    // probability that offspring inherit an allele from elite parent
	const unsigned K = 1;		                    // number of independent populations
	const unsigned MAXT = 1;	                    // number of threads for parallel decoding

	const long unsigned rngSeed = 0;	// seed to the random number generator
	MTRand rng(rngSeed);				// initialize the random number generator

    // initialize the BRKGA-based heuristic

	BRKGA< MultiwayDecoder, MTRand > algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);

    algorithm.setInitialPopulation(ini_population, ini_population.size(), 0);
   
    unsigned generation = 0;		// current generation
	const unsigned X_INTVL = 100;	// exchange best individuals at every 100 generations
	const unsigned X_NUMBER = 2;	// exchange top 2 best
	const unsigned MAX_GENS = 100;	// run for 1000 gens
    
	do {
		algorithm.evolve();	// evolve the population for one generation

		if((++generation) % X_INTVL == 0) {
			algorithm.exchangeElite(X_NUMBER);	// exchange top individuals
		}
	} while (generation < MAX_GENS);

	double cost_BRKGA = (int)algorithm.getBestFitness();
    vector<double> chromosome = algorithm.getBestChromosome();
    decoder.get_multiway_cut(chromosome, cost, cut_set);

    if (cost != (int)cost_BRKGA)
    {
        cout << "Custos diferentes! Algoritmo não está recalculando certo." << endl;
        cout << "Cromossomo: ";
        for (vector<double>::iterator it = chromosome.begin() ; it != chromosome.end(); ++it)
            cout << ' ' << *it;
        cout << endl;
        cout << "Custo BRKGA: " << cost_BRKGA << endl;
        cout << "Custo Apprx: " << cost << endl;
        cut_set.clear();
        cost = -1; cost_BRKGA = -1;
    }
}


int main(int argc, char** argv)
{
    if(argc < 3)
    {
        cout << "This code requires (first) a solver option, and (second) a instance input." << endl;
        cout << "Choose which solver you want to use:" << endl << "0: simple solver; 1: multiway1";
        cout << "Please select instance one from the instances directory. (i.e. ./multiwaybrkga steinb1.txt)" << endl;
        return 0;
    }
    
    auto start = chrono::steady_clock::now();
    string which_solver = argv[1];
    string file_name = argv[2];

    // getting num of e
    unsigned int num_of_v, num_of_e;
    string full_path = "Instances/" + file_name;
    ifstream graph_file;
    graph_file.open(full_path);
    if(graph_file.fail())
        return 0;
    graph_file >> num_of_v;
    graph_file >> num_of_e;
    graph_file.close();

    // vector of initial chromossomes
    vector<vector<double>> ini_population;

    // adicionar o cromossomo neutro
    vector<double> aux_chromosome(num_of_e, 0.5);
    ini_population.push_back(aux_chromosome);

    // aux digraph ... is to use in turn_into_chromosome, bc our multiwaycut algorithm modify the original digraph
    ListDigraph aux_digraph;
    ListDigraph::ArcMap<int> aux_capacity(aux_digraph);
    vector<int> terminals;

    // initializing aux digraph and capacity
    MultiwaySortSolver *any_solver = new MultiwaySortSolver(0);
    if(!any_solver->read_file(aux_digraph, file_name, aux_capacity, terminals))
    {
        cout << "Could not open the file." << endl;
        return 0;
    }
    delete any_solver;

    // set to store multiway solution
    set<tuple<int,int,int>> cut_ini_solutions;

    // adicionar solução do multiwaysort
    cut_ini_solutions = run_multiwaysort(file_name);

    turn_into_chromosome(aux_digraph, aux_capacity, cut_ini_solutions, aux_chromosome);
    ini_population.push_back(aux_chromosome);

    // adicionar soluções diferentes do multiway aleatorizado
    vector<int> diff_costs;
    int cut_cost = -1;
    for(int i = 0; i < 20; i++)
    {
        cut_ini_solutions = run_multiway3shuffled(file_name, cut_cost);

        // se o custo for novo
        if (find(diff_costs.begin(), diff_costs.end(), cut_cost) == diff_costs.end())
        {
            diff_costs.push_back(cut_cost);

            // getting and storing chromossome
            turn_into_chromosome(aux_digraph,aux_capacity, cut_ini_solutions, aux_chromosome);
            ini_population.push_back(aux_chromosome);
        }
    }
    cut_cost = -1;

    if (DEBUG == 1)
    {
        cout << "Qnt de custos distintos: " << diff_costs.size() << endl;
        cout << "Tam da pop inicial: " << ini_population.size() << endl;
    }

    set<pair<int, int>> cut_set;
    if (which_solver == "0")
        run_simple_BRKGA(file_name, cut_cost, cut_set, ini_population);

    if (which_solver == "1")
        run_BRKGA(file_name, cut_cost, cut_set, ini_population);


    auto end = chrono::steady_clock::now();
    chrono::duration<double> diff = end - start;
    double time_taken = diff.count();
    cout << "Time taken by program is : " << fixed
         << fixed << setprecision(2) << time_taken;
    cout << " sec " << endl;

    if (which_solver == "0")
        save_result(file_name, "BRKGA simple solver/", cut_set, cut_cost, time_taken);

    if (which_solver == "1")
        save_result(file_name, "BRKGA multiway1 solver perturb2 n=4 with ini pop/", cut_set, cut_cost, time_taken);

    return 0;
}
