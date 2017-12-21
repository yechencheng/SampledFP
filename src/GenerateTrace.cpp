#include <iostream>
#include <cstdint>
#include <cstdlib>

#include <boost/program_options.hpp>

#include "sampler.h"
#include "type.h"
#include "compressor.h"

using namespace std;
using namespace boost::program_options;

int main(int argc, char** argv){
    double spr;
    string fname;
    string outfile = "output";

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("input,i", value<string>(&fname)->required(), "input trace file")
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
    
    SimpleRandomSampler rsamp(fname, spr);
    AddrInt addr;

    Compressor cmp(outfile);

    cmp.write(spr);
    int64_t pos;
    while((pos = rsamp.next(addr)) != -1){
        addr >>= 6;
        cmp.write(pos);
        cmp.write(addr);
    }
    cmp.close();
    return 0;
}
