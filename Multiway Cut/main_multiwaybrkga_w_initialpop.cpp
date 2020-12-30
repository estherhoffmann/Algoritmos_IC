#include <iostream>
#include <chrono>
#include <iomanip>
#include <stdlib.h>
#include <fstream>

#include "Classes/SimpleMultiwayDecoder.h"
#include "Classes/MultiwayDecoder.h"
#include "BRKGA/MTRand.h"
#include "BRKGA/BRKGA.h"
/*
#include "Classes/Multiway1Solver.h"
#include "Classes/MultiwayAlternativeSolver.h"
#include "Classes/MultiwayMixSolver.h"
#include "Classes/MultiwaySortSolver.h"
*/
using namespace std;


void decoder_test(string file_name, const vector< double >& chromosome)
{
    MultiwayDecoder decoder(file_name);	
    int cost = decoder.decode(chromosome);
    cout << "Cost: " << cost << endl;
}

void turn_into_chromossome(set<tuple<int, int, int>> cut_set, vector<double> aux_chromossome)
{
    // Arestas do corte recebem 0.9 no cromossomo. Demais arestas recebem 0.1

    // Preenche aux_chromossome com o.1
    for(int i=0; i < aux_chromossome.size(); i++)
        aux_chromossome[i] = 0.1;
    
    // Para cada aresta no corte, 
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
void run_simple_BRKGA(string file_name, int& cost, set<pair<int, int>>& cut_set)
{
    SimpleMultiwayDecoder decoder(file_name);			// initialize the decoder

    int num_of_v = decoder.get_num_of_v();
    int num_of_t = decoder.get_num_of_t();

    const unsigned n = (num_of_v - num_of_t) - 1;    // size of chromosomes
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
void run_BRKGA(string file_name, int& cost, set<pair<int, int>>& cut_set)
{
    MultiwayDecoder decoder(file_name);			// initialize the decoder

    int num_of_v = decoder.get_num_of_v();
    int num_of_e = decoder.get_num_of_e();
    int num_of_t = decoder.get_num_of_t();

    const unsigned n = num_of_e - 1;	            // size of chromosomes
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

    // DECLARAR OS PARAMETROS DO BRKGA ANTESSSSS, se der ;-;
    MultiwayDecoder aux_decoder(file_name);			// aux decoder to get num_of_e

    int num_of_v = aux_decoder.get_num_of_v();
    int num_of_e = aux_decoder.get_num_of_e();
    int num_of_t = aux_decoder.get_num_of_t();

    const unsigned n = num_of_e;		            // size of chromosomes
	const unsigned p = 100;	                        // size of population

    // getting initial solutions
    set<tuple<int,int,int>> cut_ini_solutions;

    // vector of initial chromossomes
    vector<vector<double>> ini_population;
    vector<double> aux_chromossome(num_of_e, 0.5);

    ini_population.push_back(aux_chromossome);

    aux_decoder.turn_into_chromossome(cut_ini_solutions, aux_chromossome);

    return 0;

    //função que transforma as soluções em cromossomos, e insere em ini_population

    int cut_cost = 0;
    set<pair<int, int>> cut_set;

    if (which_solver == "0")
        run_simple_BRKGA(file_name, cut_cost, cut_set);

    if (which_solver == "1")
        run_BRKGA(file_name, cut_cost, cut_set);


    auto end = chrono::steady_clock::now();
    chrono::duration<double> diff = end - start;
    double time_taken = diff.count();
    cout << "Time taken by program is : " << fixed
         << fixed << setprecision(2) << time_taken;
    cout << " sec " << endl;

    if (which_solver == "0")
        save_result(file_name, "BRKGA simple solver/", cut_set, cut_cost, time_taken);

    if (which_solver == "1")
        save_result(file_name, "BRKGA multiway1 solver perturb2 n=4/", cut_set, cut_cost, time_taken);

    return 0;
}
