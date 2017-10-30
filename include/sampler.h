#include <fstream>
#include <string>
#include <random>

#include "type.h"

using namespace std;

class Sampler{
protected:
    ifstream fin;
    int64_t pos; //sampler position
public:
    Sampler(string fname);
    ~Sampler();
    virtual int64_t next(AddrInt &x)=0; // return position of sampled address, -1 if trace is ended
};

class UniformSampler : public Sampler{
private:
    int strip;
public:
    UniformSampler(string fname, int _strip);
    int64_t next(AddrInt &x);
};


class SimpleRandomSampler : public Sampler{
private:
    double spr; // sampling rate
    random_device rd;
    mt19937 gen;
    uniform_real_distribution<> dis;

public:
    SimpleRandomSampler(string fname, double _spr);
    int64_t next(AddrInt &x);
};
