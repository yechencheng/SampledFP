#include <iostream>
#include <fstream>
#include <map>
#include <unordered_set>

#include <boost/program_options.hpp>
#include <boost/circular_buffer.hpp>

#include "statistic.h"
#include "compressor.h"
#include "type.h"

using namespace std;
using namespace boost::program_options;

int main(int argc, char** argv){
    string fname;
    string output = "output.feq";
    int64_t w;

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("trace,t", value<string>(&fname)->required(), "input trace file")
        ("window,w", value<int64_t>(&w)->required(), "a file of list of y")
    ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    if(vm.count("help")){
        cerr << desc << endl;
        return 1;
    }
    notify(vm);

    Decompressor dcmp(fname);
    double sr=1.0;
    dcmp.read(sr);
    WindowFrequency wf(w/sr,w);
    HeadFrequency hf(w/sr,w);

    int64_t pos;
    AddrInt addr;

    unordered_map<AddrInt, int64_t> feq;
    while(dcmp.read(pos)){
        dcmp.read(addr);
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