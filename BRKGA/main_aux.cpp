#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "SimpleMultiwayDecoder.h"

using namespace std;

int main(int argc, char* argv[])
{
    cout << "teste??";
    SimpleMultiwayDecoder decoder("teste1.txt");

    const vector< double > teste2{0.1, 0.1, 0.0, 0, 0.2, 0.1};
    cout << "aaaa";
    double cost = decoder.decode(teste2);
    cout << cost << endl;

    return 0;
}
