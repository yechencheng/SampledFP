#include <iostream>
#include <unordered_map>
#include <cstdint>
#include <cstdlib>
#include <queue>

#include "sampler.h"
#include "type.h"

using namespace std;
typedef pair<AddrInt, int64_t> Access;
unordered_map<AddrInt, int64_t> prev_pos;

int wss = 0; //wss of the queue
int64_t current_pos = 0;
queue<Access> q;

void output_ws(int64_t wss, int64_t num=1){
    cout << wss << " " << num << endl;
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
    while(current_pos > q.front().second){
        q.pop();
    }
    
    q.push(Access(addr, pos));
    int64_t prefix = q.front().second - current_pos + 1;
    int64_t suffix = pos-current_pos-ws+1;
    int64_t step = min(prefix, suffix);
    
    while(suffix != 0 && step != 0){
        output_ws(wss, step);
        if(step == prefix && prev_pos[q.front().first] == q.front().second)
            wss--;
        q.pop();

        current_pos += step;
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
    output_ws(wss, 1);
    prev_pos[addr] = pos;
    current_pos++;
    if(prefix <= 1 && q.size() > 0){
        if(prev_pos[q.front().first] == q.front().second)
            wss--;
        q.pop();
    }
}

int main(int argc, char** argv){
    if(argc < 4){
        cerr << "usage : command trace_file window_length strip" << endl;
        return 1;
    }

    string fname = string(argv[1]);
    int ws = atoi(argv[2]);
    int strip = atoi(argv[3]);

    UniformSampler usamp(fname, strip);
    AddrInt addr;

    int64_t pos;
    while((pos = usamp.next(addr)) != -1){
        update_wss(addr, pos, ws);
    }
    return 0;
}