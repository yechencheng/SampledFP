#include <iostream>
#include <unordered_map>
#include <unordered_set>
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

    int64_t ws, strip;
    string fname;
    string outfile = "output";

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("input,i", value<string>(&fname)->required(), "input trace file")
        ("ws,w", value<int64_t>(&ws)->required(), "window size")
        ("strip,s", value<int64_t>(&strip)->required(), "strip of sampling")
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
    UniformSampler usamp(fname, strip);
    
    AddrInt addr;
    int64_t cnt = 0;
    int64_t pos;

    while((pos = usamp.next(addr)) != -1){
        if(cnt++ % 100000000 == 0) cout << cnt/100000000 << endl;
        addr >>= 6;
        wssc.update_wss(addr, pos);
    }
    wssc.close();

    return 0;
}
