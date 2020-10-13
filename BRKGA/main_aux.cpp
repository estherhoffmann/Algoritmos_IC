#include <iostream>
#include "SimpleMultiwayDecoder.h"
#include "SampleDecoder.h"

using namespace std;

int main(int argc, char* argv[])
{
    SimpleMultiwayDecoder decoder;

    //SampleDecoder decoder;

    const vector< double > teste2{1.5, 2.0};
    decoder.decode(teste2);

    return 0;
}
