#include "sampler.h"

#include <fstream>
#include <cassert>

using namespace std;

Sampler::Sampler(string fname){
    fin.open(fname, ifstream::binary);
    assert(fin.good());
    pos = 0;
}

UniformSampler::UniformSampler(string fname, int _strip) : Sampler(fname){
    strip = _strip;
}

int64_t UniformSampler::next(AddrInt &rt){
    fin.read((char*)&rt, sizeof(AddrInt));
    fin.ignore((strip-1)*sizeof(AddrInt));
    if(!fin.good()) return -1;
    pos += strip+1;
    return pos-1;
}
