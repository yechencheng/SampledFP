#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>

#include "statistic.h"

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

    ifstream fin(fname);

    int ws;
    fin.read((char*)&ws, sizeof(ws));
    CountWindow cw(ws);

    int wss;
    int64_t num;
    int nsampled;
    while(fin.read((char*)&wss, sizeof(wss))){
        fin.read((char*)&num, sizeof(num));
        fin.read((char*)&nsampled, sizeof(nsampled));
        cw.update(wss, num, nsampled);
    }
    for(auto &i : cw.cnt){
    }
    return 0;
}