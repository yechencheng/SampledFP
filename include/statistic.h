#pragma once
#include <cassert>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <utility>

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