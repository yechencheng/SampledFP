#include <iostream>
#include <fstream>
#include <map>
#include <unordered_set>
#include <queue>
#include <algorithm>

#include <boost/program_options.hpp>
#include <boost/circular_buffer.hpp>

#include "statistic.h"
#include "compressor.h"
#include "type.h"
#include "sampler.h"

using namespace std;
using namespace boost::program_options;

int main(int argc, char** argv){
    string fname;
    int64_t w;

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("input,i", value<string>(&fname)->required(), "input trace file")
        ("window,w", value<int64_t>(&w)->required(), "a file of list of y")
    ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    if(vm.count("help")){
        cerr << desc << endl;
        return 1;
    }
    notify(vm);
    
    WindowFrequency wf(w,w);
    HeadFrequency hf(w,w);

    UniformSampler usamp(fname, 1);
    queue<AddrInt> q;

    unordered_map<int64_t, int64_t> feq;

    AddrInt addr;
    int64_t pos;
    unordered_map<AddrInt, int64_t> cnt;
    while((pos = usamp.next(addr)) != -1){
        addr >>= 6;
        hf.OnReference(addr, pos);
        wf.OnReference(addr, pos);
    }
    hf.Finalize();
    wf.Finalize();

    hf.output();
    cout << string(20,'=') << endl;
    wf.output();
    
    return 0;
}