#include <iostream>
#include <unordered_map>
#include <cstdint>
#include <cstdlib>
#include <queue>

#include <boost/program_options.hpp>

#include "sampler.h"
#include "type.h"
#include "wss.h"

using namespace std;
using namespace boost::program_options;

int main(int argc, char** argv){

    int ws, strip;
    string fname;
    string outfile = "output";

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("input,i", value<string>(&fname)->required(), "input trace file")
        ("ws,w", value<int>(&ws)->required(), "window size")
        ("strip,s", value<int>(&strip)->required(), "strip of sampling")
        ("output,o", value<string>(&outfile), "output file")
    ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    if(vm.count("help")){
        cerr << desc << endl;
        return 1;
    }
    notify(vm);
    
    WSSCalculator wssc(ws, outfile);
    //fout.open(outfile, ofstream::binary | ofstream::out);
    UniformSampler usamp(fname, strip);
    AddrInt addr;

    int64_t pos;
    while((pos = usamp.next(addr)) != -1){
        wssc.update_wss(addr, pos);
    }

    return 0;
}