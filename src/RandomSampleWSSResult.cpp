#include <iostream>
#include <unordered_map>
#include <cstdint>
#include <cstdlib>
#include <queue>

#include <boost/program_options.hpp>

#include "sampler.h"
#include "type.h"
#include "wss.h"
#include "statistic.h"

using namespace std;
using namespace boost::program_options;

int main(int argc, char** argv){

    int64_t ws;
    double spr;
    string fname;
    string outfile = "";
    int64_t nwss = -1;

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("input,i", value<string>(&fname)->required(), "input trace file")
        ("ws,w", value<int64_t>(&ws)->required(), "window size")
        ("spr,s", value<double>(&spr)->required(), "sampling ratio")
        //("output,o", value<string>(&outfile), "output file")
        ("nsample,n", value<int64_t>(&nwss), "number of sampled")
    ;

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    if(vm.count("help")){
        cerr << desc << endl;
        return 1;
    }
    notify(vm);
    
    CountWindow cw(ws);
    WSSCalculator wssc(ws);
    wssc.process_ws_nsampled = bind(&CountWindow::update, &cw, placeholders::_1, placeholders::_2, placeholders::_3);
    SimpleRandomSampler rsamp(fname, spr);
    AddrInt addr;

    int64_t pos;
    while((pos = rsamp.next(addr)) != -1){
        addr >>= 6;
        wssc.update_wss(addr, pos);
    }
    wssc.close();

    if(nwss != -1){
        cout << cw.cnt[nwss].first << "\t" << cw.cnt[nwss].second << endl;
    }
    else{
        for(auto &i : cw.cnt){
            cout << i.first << "\t" << i.second.first << "\t" << i.second.second << endl;
        }
    }
    return 0;
}
