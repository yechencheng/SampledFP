#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/circular_buffer.hpp>

#include "statistic.h"
#include "compressor.h"
#include "hist.h"

using namespace std;
using namespace boost::program_options;

class FeqCounter{
private:
    HashHist feqHist;
    unordered_map<AddrInt, pair<uint64_t, uint64_t>> data;
    boost::circular_buffer<AddrInt> access;
    uint64_t ws;

    uint64_t cnt;
public:
    FeqCounter(uint64_t _ws);
    void OnReference(AddrInt addr);
    void Output();
};

FeqCounter::FeqCounter(uint64_t _ws){
    ws = _ws;
    access.resize(ws);
    cnt = 0;
}

void FeqCounter::OnReference(AddrInt addr){
    if(cnt++ < ws){
        access.push_back(addr);
        return;
    }

    AddrInt back = access.back();
    access.push_back(addr);

    auto &x = data[back];
    feqHist[x.first] += cnt-x.second;
    x.first--;
    x.second = cnt;

    if(data.find(addr) == data.end())
        data[addr] = make_pair(0,0);
    auto &y = data[addr];
    feqHist[y.first] += cnt-y.second;
    y.first++;
    y.second = cnt;
}

void FeqCounter::Output(){
    feqHist.output();
}

int main(int argc, char** argv){
    string fname;
    uint64_t ws;
    double sr = 1.0;

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("input,i", value<string>(&fname)->required(), "input trace file")
        ("window,w", value<uint64_t>(&ws)->required(), "window size")
        //("sr,s", value<double>(&sr), "sampling ratio")
    ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    if(vm.count("help")){
        cerr << desc << endl;
        return 1;
    }
    notify(vm);

    UniformSampler usample(fname, 1);
    FeqCounter feqc;

    int64_t pos;
    AddrInt addr;
    while((pos = usample.next(addr)) != -1){
        addr >>= 6;
        feqc.OnReference(addr);
    }
    feqc.output();
    return 0;
}