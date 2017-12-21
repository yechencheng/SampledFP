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

vector<int64_t> GenerateYList(){
    vector<int64_t> rt;
    //for(int64_t i = 65536; i < 786432; i += 512)
    for(int64_t i = 65536; i < 786432; i += 4096)
        rt.push_back(i);
    return rt;
}

int main(int argc, char** argv){
    string fname;
    string ylist = "";

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("trace,t", value<string>(&fname)->required(), "input trace file")
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

    vector<int64_t> y;
    vector<int64_t> x;
    if(ylist.size() == 0){
        y = GenerateYList();
    }
    else{
        ifstream yl(ylist);
        int64_t ty;
        while(yl >> ty)
            y.push_back(ty);
        yl.close();
    }

    sort(y.begin(), y.end());
    for(auto i : y)
        x.push_back(i/sr);

    int64_t pos;
    AddrInt addr;
    boost::circular_buffer<pair<int64_t,AddrInt>> cb(y.back()+20);   //pos, addr
    unordered_set<AddrInt> ndis;
    

    vector<int64_t> wss(y.size(), 0);
    vector<int64_t> cw(y.size(), 0);
    int64_t cnt = 0;

    while(dcmp.read(pos)){
        cnt++;
        dcmp.read(addr);
        cb.push_back(make_pair(pos, addr));

        for(int i = 0, j = cb.size()-2; i < y.size(); i++){
            if(cb.size() < y[i]+2) break;
            int64_t tail_id = cb.size()-2;
            int64_t head_id = cb.size()-y[i]-1;
            while(j >= head_id)
                ndis.insert(cb[j--].second);
            
            if(cb[tail_id].first - cb[head_id].first >= x[i]) continue;
            
            //compute nwindow
            int64_t start_pos = max(cb[tail_id].first - x[i] + 1, cb[head_id-1].first+1);
            int64_t end_pos = min(cb[tail_id+1].first-1, cb[head_id].first+x[i]-1);
            int64_t nw = min(cb[head_id].first-start_pos+1, end_pos-cb[tail_id].first+1);
            cw[i] += nw;
            wss[i] += nw*ndis.size();
        }
        ndis.clear();
    }

    for(int i = 0; i < y.size(); i++)
        cout << y[i] << "\t" << cw[i] << "\t" << wss[i] << "\t" << wss[i]/(double)cw[i] << endl; 
    return 0;
}