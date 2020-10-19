#include "MultiwaySolver.h"
#include "MultiwayDecoder.h"
#include <iostream>


MultiwayDecoder::SampleDecoder() { }

MultiwayDecoder::~SampleDecoder() { }

// Runs in \Theta(n \log n):
double MultiwayDecoder::decode(const std::vector< double >& chromosome) const {
	
	// sample fitness is the first allele
	return chromosome.front();
}