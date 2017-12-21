#include <iostream>
#include <fstream>
#include <map>
#include <unordered_map>

#include <boost/program_options.hpp>

#include "statistic.h"
#include "compressor.h"
#include "type.h"

using namespace std;
using namespace boost::program_options;

int main(int argc, char** argv){
    string fname;
    string ylist;

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("trace,t", value<string>(&fname)->required(), "input trace file")
        ("ylist,y", value<string>(&ylist)->required(), "a file of list of y")
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

    vector<int64_t> y;
    vector<int64_t> x;
    ifstream yl(ylist);
    int64_t ty;
    while(yl >> ty)
        y.push_back(ty);
    sort(y.begin(), y.end());
    for(auto i : y)
        x.push_back(i/sr);

    int64_t pos;
    AddrInt addr;
    boost::circular_buffer<pair<int64_t,AddrInt>> cb(y.size()*2);
    while(dcmp.read(pos)){
        dcmp.read(addr);
        cb.push_back(make_pair(pos, addr));
        
        for(auto i : y){
            if(cb.size() < i+2) break;
            
        }
    }
    return 0;
}