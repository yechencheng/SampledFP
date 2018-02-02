#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

#include "statistic.h"
#include "compressor.h"

using namespace std;
using namespace boost::program_options;


int main(int argc, char** argv){
    string fname;
    
    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("input,i", value<string>(&fname)->required(), "input trace file")
    ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    if(vm.count("help")){
        cerr << desc << endl;
        return 1;
    }
    notify(vm);

    Decompressor dcmp(fname);

    int64_t ws;
    dcmp.read(ws);
    cout << "WS : " << ws << endl;
    Footprint fp(ws);
    int wss;
    int64_t num = 0;
    int nsampled = 0;
    while(dcmp.read(wss)){
        dcmp.read(num);
        dcmp.read(nsampled);
        fp.update(wss, num, nsampled);
        assert(nsampled == ws);
    }
    cout << fp.nw << "\t" << fp.sum << endl;
    return 0;
}
