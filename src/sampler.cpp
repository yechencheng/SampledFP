#include "sampler.h"

#include <fstream>
#include <assert>

using namespace std;

Sampler::Sampler(string fname){
    fin.open(fname, ifstream::binary);
    assert(fin.good());
}

UniformSampler::UniformSampler(string fname, int _strip) : Sampler(fname){
    strip = _strip;
}

bool UniformSampler::next(AddrInt &rt){
    AddrInt a[strip];
    fin.read((char*)a, strip*sizeof(AddrInt));
    rt = a[0];
    return fin.good();
}
