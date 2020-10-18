#include <iostream>
#include <chrono>
#include <iomanip>
#include <stdlib.h>
#include "SimpleMultiwayDecoder.h"

using namespace std;

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cout << "This code requires a instance input. ";
        cout << "Please select one from the instances directory. (i.e. ./multiway1 steinb1.txt)";
        return 0;
    }

    auto start = chrono::steady_clock::now();

    string file_name = argv[1];
    SimpleMultiwayDecoder decoder(file_name);
    int num_of_v = decoder.get_num_of_v();
    int num_of_t = decoder.get_num_of_t();

    vector< double > chromossome;
    cout << "Chromossome: ";
    for(int i=0; i<(num_of_v - num_of_t); i++)
    {
        chromossome.push_back(((double) rand() / (RAND_MAX)));
        cout << chromossome[i] << ", ";
    }

    double cost = decoder.decode(chromossome);
    cout << "\nCusto da solução: " << cost << endl;

    auto end = chrono::steady_clock::now();
    chrono::duration<double> diff = end - start;
    double time_taken = diff.count();
    cout << "Time taken by program is : " << fixed
         << fixed << setprecision(2) << time_taken;
    cout << " sec " << endl;



    return 0;
}
