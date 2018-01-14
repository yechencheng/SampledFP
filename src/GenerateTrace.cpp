#include <iostream>
#include <cstdint>
#include <cstdlib>

#include <boost/program_options.hpp>

#include "sampler.h"
#include "type.h"
#include "compressor.h"

using namespace std;
using namespace boost::program_options;

void saw(){
    int64_t len = 100000000ll;
    vector<AddrInt> addr;
    addr.push_back(1<<10);
    addr.push_back(2<<10);
    addr.push_back(3<<10);
    addr.push_back(3<<10);
    addr.push_back(2<<10);
    addr.push_back(1<<10);

    Compressor cmp("saw.trace");
    for(int64_t i = 0; i < len; i++){
        cmp.write(addr[i%6]);
    }
    cmp.close();
}

int main(int argc, char** argv){
    double spr;
    string fname;
    string outfile = "output";
    string func = "";

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("input,i", value<string>(&fname), "input trace file")
        ("spr,s", value<double>(&spr), "sampling ratio")
        ("output,o", value<string>(&outfile), "output file")
        ("function,f", value<string>(&func), "call function")
    ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    if(vm.count("help")){
        cerr << desc << endl;
        return 1;
    }
    notify(vm);
    if(func == "saw"){
        saw();
        return 0;
    }

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
