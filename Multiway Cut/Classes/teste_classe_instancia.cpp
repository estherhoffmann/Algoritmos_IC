#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <iomanip>
#include <chrono>
#include "MultiwayInstance.h"

int DEBUG = 0;
int DEBUG_RESULT = false;

using namespace std;

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        cout << "This code requires a instance input. ";
        cout << "Please select one from the instances directory. (i.e. ./multiway1 steinb1.txt)";
        return 0;
    }

    // 'start counting time'
    auto start = chrono::steady_clock::now();

    string file_name = argv[1];
    MultiwayInstance graph(file_name);

    cout << "Num of edges: " << graph.get_num_of_e() << endl;
    cout << "Num of vertices: " << graph.get_num_of_v() << endl;
    cout << "Num of terminals: " << graph.get_num_of_t() << endl;
    
    // Calculating total time taken by the program.
    auto end = chrono::steady_clock::now();
    chrono::duration<double> diff = end - start;
    double time_taken = diff.count();
    cout << "Time taken by program is : " << fixed
         << fixed << setprecision(2) << time_taken;
    cout << " sec " << endl;

    return 0;
}
