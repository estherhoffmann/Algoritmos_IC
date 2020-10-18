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

class SimpleMultiwayDecoder {
private:
    int num_of_v;
    int num_of_e;
    int num_of_t;
    std::vector<int> terminals;
    std::map<std::pair<int, int>, int> edges;

public:
	SimpleMultiwayDecoder(std::string);
	~SimpleMultiwayDecoder();

	double decode(const std::vector< double >&);
    bool read_file(std::string);

    int get_num_of_v();
    int get_num_of_e();
    int get_num_of_t();
    std::vector<int> get_terminals();
    int get_edge_cost(std::pair<int, int>);
};

#endif
