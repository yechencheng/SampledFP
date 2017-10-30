#include <iostream>
#include <unordered_map>
#include <cstdint>
#include <cstdlib>
#include <queue>

#include <boost/program_options.hpp>

#include "sampler.h"
#include "type.h"

using namespace std;
using namespace boost::program_options;

typedef pair<AddrInt, int64_t> Access;
unordered_map<AddrInt, int64_t> prev_pos;

int wss = 0; //wss of the queue
int64_t current_pos = 0;
queue<Access> q;

ofstream fout;
void output_ws(int64_t wss, int64_t num=1){
    if(num == 1) wss = -wss;
    fout.write((char*)&wss, sizeof(wss));
    if(num != 1)
        fout.write((char*)&num, sizeof(wss));
    //cout << wss << " " << num << endl;
}

void output_ws_nsampled(int64_t wss, int64_t num = 1, int nsampled = 1){
    fout.write((char*)&wss, sizeof(wss));
    fout.write((char*)&num, sizeof(num));
    fout.write((char*)&nsampled, sizeof(nsampled));
}

void update_wss(AddrInt addr, int64_t pos, int ws){
    int64_t len = pos-current_pos+1;
    if(len < ws){
        if(prev_pos.find(addr) == prev_pos.end())
            wss++;
        else
            wss += (prev_pos[addr] < current_pos);
        prev_pos[addr] = pos;
        q.push(Access(addr, pos));
        return;
    }
    while(q.size() != 0 && current_pos > q.front().second){
        q.pop();
    }
    
    q.push(Access(addr, pos));
    int64_t prefix = q.front().second - current_pos + 1;
    int64_t suffix = pos-current_pos-ws+1;
    int64_t step = min(prefix, suffix);
    
    while(suffix != 0 && step != 0){
        //output_ws(wss, step);
        output_ws_nsampled(wss, step, q.size()-1);

        if(step == prefix && prev_pos[q.front().first] == q.front().second){
            wss--;
        }
        current_pos += step;
        if(current_pos > q.front().second)
            q.pop();
        prefix = q.front().second - current_pos + 1;
        suffix -= step;
        step = min(prefix, suffix);
    }

    //process the last one
    if(suffix == 0){
        if(prev_pos.find(addr) == prev_pos.end())
            wss++;
        else if(prev_pos[addr] < current_pos)
            wss++;
    }
    //output_ws(wss, 1);
    output_ws_nsampled(wss, 1, q.size());
    prev_pos[addr] = pos;
    current_pos++;
    if(prefix <= 1 && q.size() > 0){
        if(prev_pos[q.front().first] == q.front().second)
            wss--;
        q.pop();
    }
}

int main(int argc, char** argv){

    int ws, strip;
    string fname;
    string outfile = "output";

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("input,i", value<string>(&fname)->required(), "input trace file")
        ("ws,w", value<int>(&ws)->required(), "window size")
        ("strip,s", value<int>(&strip)->required(), "strip of sampling")
        ("output,o", value<string>(&outfile), "output file")
    ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    if(vm.count("help")){
        cerr << desc << endl;
        return 1;
    }
    notify(vm);
    
    fout.open(outfile, ofstream::binary | ofstream::out);
    UniformSampler usamp(fname, strip);
    AddrInt addr;

    int64_t pos;
    while((pos = usamp.next(addr)) != -1){
        update_wss(addr, pos, ws);
    }

    fout.close();
    return 0;
}