#include <iostream>
#include <chrono>
#include <iomanip>
#include <stdlib.h>
#include <fstream>
#include "Classes/SimpleMultiwayDecoder.h"
#include "BRKGA/MTRand.h"
#include "BRKGA/BRKGA.h"

using namespace std;


void save_result(string file_name, set<pair<int, int>> cut_list, int multiway_cut_cost, double exec_time)
{
    string full_path = "Solutions/BRKGA simple solver/" + file_name.substr(0, file_name.find(".")) + ".sol";
    cout << full_path << endl;
    ofstream sol_file(full_path);

    sol_file << "custo " << multiway_cut_cost << endl;
    sol_file << "tempo " << fixed << setprecision(2) << exec_time << endl;

    for (auto const &edge : cut_list) {
        sol_file << edge.first << " " << edge.second << endl;
    }

    sol_file.close();
}

void run_BRKGA(string file_name)
{
    auto start = chrono::steady_clock::now();

    SimpleMultiwayDecoder decoder(file_name);			// initialize the decoder

    int num_of_v = decoder.get_num_of_v();
    int num_of_t = decoder.get_num_of_t();

    const unsigned n = (num_of_v - num_of_t);		// size of chromosomes
	const unsigned p = 1000;	                    // size of population
	const double pe = 0.20;		                    // fraction of population to be the elite-set
	const double pm = 0.10;		                    // fraction of population to be replaced by mutants
	const double rhoe = 0.70;	                    // probability that offspring inherit an allele from elite parent
	const unsigned K = 3;		                    // number of independent populations (((não sei doq se trata)))
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


    auto end = chrono::steady_clock::now();
    chrono::duration<double> diff = end - start;
    double time_taken = diff.count();

    int cost = (int)algorithm.getBestFitness();
    vector<double> solution = algorithm.getBestChromosome();
    set<pair<int, int>> cut_list = decoder.get_cut_list(solution);
    save_result(file_name, cut_list, cost, time_taken);

}


int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cout << "This code requires a instance input. ";
        cout << "Please select one from the instances directory. (i.e. ./multiway1 steinb1.txt)";
        return 0;
    }

    string file_name = argv[1];

    run_BRKGA(file_name);

    return 0;
}
