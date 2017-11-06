#include "sampler.h"

#include <fstream>
#include <cassert>

#include "compressor.h"

using namespace std;

Sampler::Sampler(string fname){
    dcmp = new Decompressor(fname);
    pos = 0;
}
Sampler::~Sampler(){
    dcmp->close();
}

UniformSampler::UniformSampler(string fname, int64_t _strip) : Sampler(fname){
    strip = _strip;
}

int64_t UniformSampler::next(AddrInt &rt){
    if(!dcmp->read(rt)) return -1;
    AddrInt x;
    for(int64_t i = 1; i < strip; i++)
        if(!dcmp->read(x)) return -1;
    pos += strip;
    return pos-strip;
}

SimpleRandomSampler::SimpleRandomSampler(string fname, double _spr) : Sampler(fname){
    spr = _spr;
    gen = mt19937(rd());
    dis = uniform_real_distribution<>(0.0, 1.0);
}

#include <iostream>
using namespace std;

int64_t SimpleRandomSampler::next(AddrInt &rt){
    int64_t n = 0;
    while(dis(gen) > spr)
        n++;
    if(!dcmp->read(rt)) return -1;
    AddrInt x;
    for(int64_t i = 0; i < n; i++){
        if(!dcmp->read(x)) return -1;
    }
    pos += n+1;
    return pos-n-1;
}
