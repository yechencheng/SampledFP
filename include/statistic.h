#pragma once
#include <cassert>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <utility>
#include <queue>

#include <boost/circular_buffer.hpp>

#include "type.h"
#include "hist.h"

using namespace std;

class AbstractStat{
public:
    int64_t *stat;
    int64_t ws;
    AbstractStat(int64_t _ws) : ws(_ws) {};
    virtual void update(int wss, int64_t num, int nsampled) = 0;
};

//Count windows precisely
class SimpleStatistic : public AbstractStat{
public:    
    SimpleStatistic(int64_t _ws) : AbstractStat(_ws) {
        stat = new int64_t[ws+1];
        fill(stat, stat+ws, 0);
    }

    void update(int wss, int64_t num, int nsampled){
        assert(wss <= ws);
        stat[wss] += num;
    }
};

//Count 100 windows
class CoarseStatistic : public AbstractStat{
public:
    CoarseStatistic(int64_t _ws) : AbstractStat(_ws) {
        stat = new int64_t[101];
        fill(stat, stat+101, 0);        
    }

    void update(int wss, int64_t num, int nsampled){
        assert(wss <= ws);
        stat[int(wss*1.0/ws)] += num;
    }
};

class CountWindow : public AbstractStat{
public:
    map<int,pair<int64_t, int64_t>> cnt;
    CountWindow(int64_t _ws) : AbstractStat(_ws){
    }
    
    void update(int wss, int64_t num, int nsampled){
        if(cnt.find(nsampled) == cnt.end())
            cnt[nsampled] = pair<int64_t, int64_t>(num, wss*num);
        else{
            pair<int64_t, int64_t> &x = cnt[nsampled];
            x.first += num;
            x.second += wss*num;
        }
    }
};

class Footprint : public AbstractStat{
public:
    int64_t sum;
    int64_t nw;
    Footprint(int64_t _ws) : AbstractStat(_ws){
        sum = 0;
        nw = 0;
    }
    void update(int wss, int64_t num, int nsampled){
        sum += wss*num;
        nw += num;
    }
};

class SampledFootprint{
private:
    boost::circular_buffer<pair<int64_t,AddrInt>> cb; //pos, addr
    unordered_map<AddrInt, int64_t> prev;
public:
    int64_t x;
    int64_t y;

    int64_t fp;
    int64_t wss;
    int64_t cw;
    double dfp;

    SampledFootprint(int64_t _x, int64_t _y){
        x = _x;
        y = _y;
        cb = boost::circular_buffer<pair<int64_t,AddrInt>>(y+20);
        wss = 0;
        cw = 0;
        fp = 0;
    }
    void OnReference(AddrInt addr, int64_t pos){
        cb.push_back(make_pair(pos, addr));

        int64_t tail_id = cb.size()-2;
        int64_t head_id = cb.size()-y-1;    
        if(head_id > 0 && cb[tail_id].first - cb[head_id].first <= x){
                //compute nwindow
                int64_t start_pos = max(cb[tail_id].first - x + 1, cb[head_id-1].first+1);
                int64_t end_pos = min(cb[tail_id+1].first-1, cb[head_id].first+x-1);
                int64_t nw = min(cb[head_id].first-start_pos+1, end_pos-cb[tail_id].first+1);
                cw += nw;
                wss += nw*fp;
                //cout << nw << " " << fp << endl;
        }
        if(head_id >= 0 && prev[cb[head_id].second] == cb[head_id].first)
                fp--;

        int64_t dist = -1;
        if(head_id >= 0)
            dist = cb[head_id].first;
        if(prev.find(addr) == prev.end() || prev[addr] <= dist)
            fp++;

        //cout << addr << "\t" << pos << " " << fp << " : " << head_id << " " << cb[head_id].first << " " << prev[cb[head_id].second] << endl;

        prev[addr] = pos;
    }

    void Finalize(){
        fp = wss/cw;
        dfp = wss/(double)cw;
    }
};


class HeadFrequency{
public:
    int64_t x;
    int64_t y;
    queue<AddrInt> q;
    unordered_map<AddrInt, int64_t> cnt;
    map<int64_t, int64_t> feq;

    vector<pair<int64_t,double>> fratio;
    HeadFrequency(int64_t _x, int64_t _y){
        x = _x;
        y = _y;
    }

    void OnReference(AddrInt addr, int64_t pos){
        cnt[addr]++;
        q.push(addr);
        if(q.size() == (size_t)y){            
            feq[cnt[addr]]++;
            AddrInt xaddr = q.front();
            cnt[xaddr]--;
            q.pop();
        }
    }

    void Finalize(){
        int64_t feq_sum = 0;
        for(auto i : feq){
            fratio.push_back(make_pair(i.first,i.second));
            feq_sum += i.second;
        }
        for(auto &i : fratio)
            i.second /= feq_sum;
    }

    void output(){
        for(auto i : feq)
            cout << i.first << "\t" << i.second << endl;
    }
};

class WindowFrequency{
public:
    int64_t x;
    int64_t y;
    queue<AddrInt> q;
    map<int64_t, int64_t> feq;
    unordered_map<AddrInt, pair<int64_t,int64_t>> cnt;
    int64_t nw;

    vector<pair<int64_t, double>> fratio;
    WindowFrequency(int _x, int _y){
        x = _x;
        y = _y;
        nw = 0;
    }

    void OnReference(AddrInt addr, int64_t pos){
        if(cnt.find(addr) == cnt.end())
            cnt[addr] = make_pair(0,nw);

        if(q.size() < (size_t)y){
            auto nc = cnt[addr];
            cnt[addr] = make_pair(nc.first+1, y-1);
        }
        else{
            auto& p = cnt[addr];
            feq[p.first] += nw-p.second;
            p.first++;
            p.second = nw;
            AddrInt victim = q.front();
            q.pop();
            auto &t = cnt[victim];
            if(t.second != nw)
                feq[t.first] += nw-t.second;
            t.first--;
            t.second = nw;
        }
        q.push(addr);
        nw++;
    }

    void Finalize(){
        for(auto i : cnt)
            feq[i.second.first] += nw-i.second.second;
        feq.erase(0);
        int64_t feq_sum = 0;
        for(auto i : feq)
            feq_sum += i.second;
        for(auto i : feq){
            fratio.push_back(make_pair(i.first,i.second/(double)feq_sum));
        }
    }

    void output(){
        for(auto i : feq)
            cout << i.first << "\t" << i.second << endl;
    }
};
