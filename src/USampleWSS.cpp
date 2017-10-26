#include <iostream>
#include <unordered_map>
#include <cstdint>
#include <cstdlib>
#include <queue>

#include "sampler.h"
#include "type.h"

using namespace std;

unordered_map<AddrInt, int64_t> prev_pos;
queue<AddrInt> q;

int main(int argc, char** argv){
    if(argc < 4){
        cerr << "usage : command trace_file window_length strip" << endl;
        return 1;
    }

    string fname = string(argv[1]);
    int ws = atoi(argv[2]);
    int strip = atoi(argv[3]);

    UniformSampler usamp(fname, strip);
    AddrInt addr;

    int64_t pos = 0;
    int wss = 0;
    while(usamp.next(addr)){
        if(q.size() * strip > ws){

        }
        q.push_back(addr);
        prev_pos[addr] = pos;
        pos += strip;
    }
    return 0;
}