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
    
    vector<int64_t> wss(y.size(), 0);
    vector<int64_t> cw(y.size(), 0);

    while(dcmp.read(pos)){
        dcmp.read(addr);
        cb.push_back(make_pair(pos, addr));
        
        for(int i = 0; i < y.size(); i++){
            if(cb.size() < y[i]+2) break;
            int64_t tail_id = cb.size()-2;
            int64_t head_id = cb.size()-y[i]-1;
            if(cb[tail_id].first - cb[head_id].first >= x[i]) continue;
            
            int64_t start_pos = max(cb[tail_id].first - x[i], cb[head_id-1].first+1);
            int64_t end_pos = min(cb[tail_id+1].first-1, cb[head_id].first+x[i]);
            int64_t nw = min(cb[head_id].first-start_pos+1, end_pos-cb[tail_id].first+1);
            cw[i] += nw;
            wss[i] += nw*;
        }
    }

    for(int i = 0; i < y.size(); i++)
        cout << y[i] << "\t" << cw[i] << "\t" << wss[i] << "\t" << wss[i]/(double)nw[i] << endl; 
    return 0;
}