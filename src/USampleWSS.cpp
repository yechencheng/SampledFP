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
        wss += (prev_pos[addr] < current_pos);
        prev_pos[addr] = pos;
        q.push(Access(addr, pos));
        return;
    }
    Access front = q.front();
    Access back = q.back();

    int64_t prefix = front.second - current_pos + 1;
    int64_t suffix = pos - current_pos - ws;
    int64_t step = min(prefix, suffix);
    output_ws(wss, step);
    current_pos += step;
    prefix -= step;
    suffix -= step;
    
    if(prefix == 0){
        q.pop();
        if(prev_pos[front.first] == front.second)
            wss--;
        current_pos++;
    }
    if(suffix == 0){
        q.push(Access(addr, pos));
        if(prev_pos[addr] < q.front().second)
            wss++;
        output_ws(wss, 1);
        return;
    }
    update_wss(addr, pos, ws);
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