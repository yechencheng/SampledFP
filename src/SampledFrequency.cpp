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
    string ylist = "";
    string output = "output.feq";

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("trace,t", value<string>(&fname)->required(), "input trace file")
        ("output,o", value<string>(&output), "output to a file")
        ("ylist,y", value<string>(&ylist), "a file of list of y")
    ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    if(vm.count("help")){
        cerr << desc << endl;
        return 1;
    }
    notify(vm);

    Decompressor dcmp(fname);
    double sr;
    dcmp.read(sr);
    int64_t pos;
    AddrInt addr;

    unordered_map<AddrInt, int64_t> feq;
    while(dcmp.read(pos)){
        dcmp.read(addr);
        feq[addr]++;
    }

    map<int64_t, int64_t> summary;
    for(auto i : feq)
        summary[i.second]++;
    
    ofstream fout(output);
    for(auto i : summary)
        fout << i.first << "\t" << i.second << "\n";
    return 0;
}