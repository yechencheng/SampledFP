#include "sampler.h"

#include <fstream>
#include <cassert>

using namespace std;

Sampler::Sampler(string fname){
    fin.open(fname, ifstream::binary);
    assert(fin.good());
    pos = 0;
}
Sampler::~Sampler(){
    fin.close();
}

UniformSampler::UniformSampler(string fname, int _strip) : Sampler(fname){
    strip = _strip;
}

int64_t UniformSampler::next(AddrInt &rt){
    if(!fin.good()) return -1;
    fin.read((char*)&rt, sizeof(AddrInt));
    fin.ignore((strip-1)*sizeof(AddrInt));
    pos += strip;
    return pos-strip;
}

SimpleRandomSampler::SimpleRandomSampler(string fname, double _spr) : Sampler(fname){
    spr = _spr;
    gen = mt19937(rd());
    dis = uniform_real_distribution<>(0.0, 1.0);
}

int64_t SimpleRandomSampler::next(AddrInt &rt){
    if(!fin.good()) return -1;

    int n = 0;
    while(dis(gen) > spr)
        n++;

    fin.read((char*)&rt, sizeof(AddrInt));
    fin.ignore(n*sizeof(AddrInt));
    pos += n+1;
    return pos-n-1;
}