#ifndef MULTIWAYDECODER_H
#define MULTIWAYDECODER_H
#include <map>
#include <vector>
#include <string>
#include <set>

class MultiwayDecoder {
private:
    std::string file;
	int num_of_v;
	int num_of_e;
	int num_of_t;
	
public:
	MultiwayDecoder(std::string);
	~MultiwayDecoder();

	double decode(const std::vector< double >&) const;
	int get_num_of_v() const;
	int get_num_of_e() const;
	int get_num_of_t() const;
};

#endif