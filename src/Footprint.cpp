#include <iostream>
#include <unordered_map>
#include <cstdint>
#include <cstdlib>
#include <queue>

#include <boost/program_options.hpp>

#include "sampler.h"
#include "type.h"
#include "wss.h"

#include "footprint.h"
#include "footprintManager.h"

using namespace std;
using namespace boost::program_options;

int main(int argc, char** argv){
    string fname;
    string outfile = "output";

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("input,i", value<string>(&fname)->required(), "input trace file")
        ("output,o", value<string>(&outfile), "output file, output will be a histogram")
    ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    if(vm.count("help")){
        cerr << desc << endl;
        return 1;
    }
    notify(vm);
    
    UniformSampler usamp(fname, 1);
    FPBuilder fpb;
    AddrInt addr;

    int64_t pos;
    while((pos = usamp.next(addr)) != -1){
        addr >>= 6;
        //if(cnt++ % 1000000 == 0) cout << cnt/1000000 << endl;
        fpb.OnReference(addr);
    }
    FPDist* fpd = fpb.Finalize();
    FPUtil::serialize(fpd, outfile);
    return 0;
}
