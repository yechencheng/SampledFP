#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>

#include <boost/program_options.hpp>

#include "sampler.h"
#include "statistic.h"
#include "compressor.h"

using namespace std;
using namespace boost::program_options;

unordered_map<AddrInt, int64_t> cnt;
map<int64_t, int64_t> feq;

int main(int argc, char** argv){
    string fname;
    double sr = 1.0;

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("input,i", value<string>(&fname)->required(), "input trace file")
        ("sr,s", value<double>(&sr), "sampling ratio")
    ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    if(vm.count("help")){
        cerr << desc << endl;
        return 1;
    }
    notify(vm);

    SimpleRandomSampler rsamp(fname, sr);
    int64_t pos;
    AddrInt addr;
    while((pos = rsamp.next(addr)) != -1){
        addr >>= 6;
        cnt[addr]++;
    }
    for(auto i : cnt)
        feq[i.second]++;
    for(auto i : feq)
        cout << i.first << "\t" << i.second << endl;
    return 0;
}