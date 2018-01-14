#include <iostream>
#include <fstream>
#include <map>
#include <unordered_set>
#include <queue>
#include <algorithm>

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
    //rt.push_back(65536);
    return rt;
}

int main(int argc, char** argv){
    string fname;
    int64_t w;

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("input,i", value<string>(&fname)->required(), "input trace file")
        ("window,w", value<int64_t>(&w)->required(), "a file of list of y")
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

    queue<AddrInt> q;
    unordered_map<int64_t, int64_t> feq;
    unordered_map<AddrInt, int64_t> cnt;

    int64_t pos;
    AddrInt addr;
    while(dcmp.read(pos)){
        dcmp.read(addr);

        q.push(addr);
        cnt[addr]++;
        if(q.size() != w){
            continue;
        }
        feq[cnt[addr]]++;
        AddrInt x = q.front();
        q.pop();
        cnt[x]--;        
    }

    vector<pair<int64_t, int64_t>> rt;
    for(auto i : feq)
        rt.push_back(i);
    sort(rt.begin(), rt.end());
    for(auto i : rt){
        cout << i.first << "\t" << i.second << endl;
    }

    return 0;
}