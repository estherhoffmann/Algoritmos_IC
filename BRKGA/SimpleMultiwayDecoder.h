/*
 * SimpleMultiwayDecoder.h
 *
 * Any decoder must have the format below, i.e., implement the method decode(std::vector< double >&)
 * returning a double corresponding to the fitness of that vector. If parallel decoding is to be
 * used in the BRKGA framework, then the decode() method _must_ be thread-safe; the best way to
 * guarantee this is by adding 'const' to the end of decode() so that the property will be checked
 * at compile time.
 *
 * The chromosome inside the BRKGA framework can be changed if desired. To do so, just use the
 * first signature of decode() which allows for modification. Please use double values in the
 * interval [0,1) when updating, thus obeying the BRKGA guidelines.
 */

#ifndef SIMPLEMULTIWAYDECODER_H
#define SIMPLEMULTIWAYDECODER_H

#include <list>
#include <vector>
#include <algorithm>

class SimpleMultiwayDecoder {
public:
	SimpleMultiwayDecoder();
	~SimpleMultiwayDecoder();

	double decode(const std::vector< double >& chromosome) const;
    bool SimpleMultiwayDecoder::read_file(string file_name);

    int get_num_of_v();
    int get_num_of_e();
    int get_qnt_terminals();
    vector get_terminals();
    int get_edge_cost(std::pair<int, int> edge);

private:
    int num_of_v;
    int num_of_e;
    int qnt_terminals;
    vector terminals;
    std::map<std::pair<int, int>, int> edges_cost;

};

#endif
