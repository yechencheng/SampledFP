#include <iostream>
#include <unordered_map>
#include <cstdint>
#include <cstdlib>
#include <queue>

#include "type.h"

using namespace std;
typedef pair<AddrInt, int64_t> Access;
unordered_map<AddrInt, int64_t> prev_pos;

int wss = 0; //wss of the queue
int64_t current_pos = 0;
queue<Access> q;

void output_ws(int64_t wss, int64_t num=1){
    //cout << wss << " " << num << endl;
    while(num--)
        cout << wss << " ";
}
void output_ws_nsampled(int64_t wss, int64_t num = 1, int nsampled = 1){
    cout << wss << " " << num << " " << nsampled << endl;
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

unordered_map<AddrInt, int64_t> px;
queue<AddrInt> qx;
int wssx = 0;
void burst(AddrInt addr, int64_t pos, int ws){
    if(px.find(addr) == px.end() || px[addr] <= pos-ws)
        wssx++;
    px[addr] = pos;
    qx.push(addr);
    if(qx.size() < ws) return;

    if(px.find(0) != px.end() && px[0] > pos-ws)
        output_ws(wssx-1, 1);
    else
        output_ws(wssx, 1);
    if(px[qx.front()] == pos-ws+1)
        wssx--;
    qx.pop();
}

int main(){
    AddrInt a[] = {1,2,3,3,2,1,1,2,3};
    int64_t pos[] = {0,2,3,5,6,7,8,9,15};
    int len = sizeof(a)/sizeof(AddrInt);
    int ws = 4;

    for(int i = 0; i < len; i++)
        cout << a[i] << "\t" << pos[i] << endl;
    
    for(int i = 0; i < len; i++){
        //cout << "NUM : " << a[i] << "\t" << pos[i] << endl;
        update_wss(a[i], pos[i], ws);
    }
    cout << endl;
    
    return 0;

    for(int i = 0, p = 0; i < len; i++){
        while(p < pos[i])
            burst(0, p++, ws);
        burst(a[i], p++, ws);
    }
    cout << endl;
    return 0;
}