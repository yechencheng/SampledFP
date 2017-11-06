#include <iostream>
#include <unordered_map>
#include <cstdint>
#include <cstdlib>
#include <queue>

#include "type.h"
#include "wss.h"
#include "compressor.h"

using namespace std;

void output_ws(int64_t wss, int64_t num=1){
    while(num--)
        cout << wss << " ";
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
    int ws = 5;
    WSSCalculator wssc(ws, "test");
    for(int i = 0; i < len; i++)
        cout << a[i] << "\t" << pos[i] << endl;
    
    for(int i = 0; i < len; i++){
        //cout << "NUM : " << a[i] << "\t" << pos[i] << endl;
        wssc.update_wss(a[i], pos[i]);
    }
    cout << endl;
    
    //return 0;

    for(int i = 0, p = 0; i < len; i++){
        while(p < pos[i])
            burst(0, p++, ws);
        burst(a[i], p++, ws);
    }
    cout << endl;
    return 0;
}
