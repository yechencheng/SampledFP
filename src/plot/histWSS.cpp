#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

#include "statistic.h"
#include "compressor.h"

using namespace std;
using namespace boost::program_options;

int main(int argc, char** argv){
    string fname;
    int64_t nwss = -1;

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("input,i", value<string>(&fname)->required(), "input trace file")
        ("nsample,n", value<int64_t>(&nwss), "number of sampled")
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
    CountWindow cw(ws);

    int wss;
    int64_t num;
    int nsampled;
    while(dcmp.read(wss)){
        dcmp.read(num);
        dcmp.read(nsampled);
        cw.update(wss, num, nsampled);
    }
    //cout << cw.cnt.size() << endl;
    if(nwss != -1){
        cout << cw.cnt[nwss].first << "\t" << cw.cnt[nwss].second << endl;
        return 0;
    }
    for(auto &i : cw.cnt){
        cout << i.first << "\t" << i.second.first << "\t" << i.second.second << endl;
    }
    return 0;
}
