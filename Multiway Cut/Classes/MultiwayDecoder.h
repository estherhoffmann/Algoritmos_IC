#ifndef MULTIWAYDECODER_H
#define MULTIWAYDECODER_H
#include <map>
#include <vector>
#include <string>
#include <set>

class MultiwayDecoder {
private:
    std::string file_name;
public:
	MultiwayDecoder(std::string);
	~MultiwayDecoder();

	double decode(const std::vector< double >&) const;
};

#endif