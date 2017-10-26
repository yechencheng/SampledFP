#include <fstream>
#include <string>

#include "type.h"

using namespace std;

class Sampler{
private:
    ifstream fin;
public:
    Sampler(string fname);
    bool next(AddrInt &x)=0; // return true if not hit the end of the trace
};

class UniformSampler : public Sampler{
private:
    int strip;
public:
    UniformSampler(string fname);
    bool next(AddrInt &x);
};

/*
class SimpleRandomSampler : public Sampler{
private:

public:
    SimpleRandomSampler(string fname);
    bool next(AddrInt &x);
}
*/