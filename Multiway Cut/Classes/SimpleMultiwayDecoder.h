#ifndef SIMPLEMULTIWAYDECODER_H
#define SIMPLEMULTIWAYDECODER_H
#include <map>
#include <vector>
#include <string>
#include <set>

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

	double decode(const std::vector< double >&) const;
    bool read_file(std::string);

    int get_num_of_v();
    int get_num_of_e();
    int get_num_of_t();
    std::vector<int> get_terminals();
    int get_edge_cost(std::pair<int, int>);
    std::set<std::pair<int,int>> get_cut_list(const std::vector<double>&) const;

};

#endif
