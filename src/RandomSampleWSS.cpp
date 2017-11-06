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

    int64_t ws;
    double spr;
    string fname;
    string outfile = "output";

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("input,i", value<string>(&fname)->required(), "input trace file")
        ("ws,w", value<int64_t>(&ws)->required(), "window size")
        ("spr,s", value<double>(&spr)->required(), "sampling ratio")
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
    SimpleRandomSampler rsamp(fname, spr);
    AddrInt addr;

    int64_t cnt = 0;
    int64_t pos;
    while((pos = rsamp.next(addr)) != -1){
        addr >>= 6;
        if(cnt++ % 100000 == 0) cout << cnt/100000 << endl;
        wssc.update_wss(addr, pos);
    }
    wssc.close();

    return 0;
}
