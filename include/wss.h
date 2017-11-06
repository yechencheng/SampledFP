#pragma once
#include <iostream>
#include <unordered_map>
#include <cstdint>
#include <cstdlib>
#include <queue>
#include <string>
#include <fstream>

#include "type.h"
#include "compressor.h"

using namespace std;

class WSSCalculator{
private:
    typedef pair<AddrInt, int64_t> Access;
    unordered_map<AddrInt, int64_t> prev_pos;
    
    int64_t ws; //window size
    int wss; //wss of the queue
    int64_t current_pos;
    queue<Access> q;
    
    Compressor *cmp;
public:
    WSSCalculator(int64_t _ws, string fname){
        ws = _ws;
        wss = 0;
        current_pos = 0;
        cmp = new Compressor(fname);
        cmp->write(ws);
    }

    void close(){
        cmp->close();
    }
    ~WSSCalculator(){
        close();
    }

    void output_ws(int wss, int64_t num=1){
        cmp->write(wss);
        cmp->write(num);
        //cout << wss << " " << num << endl;
        //while(num--)
        //    cout << wss << " ";
    }
    
    void output_ws_nsampled(int wss, int64_t num = 1, int nsampled = 1){
        cmp->write(wss);
        cmp->write(num);
        cmp->write(nsampled);
    }

    void update_wss(AddrInt addr, int64_t pos){
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
};
