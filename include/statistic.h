#pragma once
#include <cassert>

class AbstractStat{
public:
    int64_t *stat;
    int ws;
    AbstractStat(int _ws) : ws(_ws) {};
    virtual void update(int wss, int num) = 0;
};

class Statistic : public AbstractStat{
public:    
    Statistic(int _ws){
        stat = new int64_t[ws+1];
        fill(stat, stat+ws, 0);
    }

    void update(int wss, int num){
        assert(wss <= ws);
        stat[wss] += num;
    }
};

class CoarseStatistic : public AbstractStat{
public:
    CoarseStatistic(int _ws){
        stat = new int64_t[101];
        fill(stat, stat+101, 0);        
    }

    void update(int wss, int num){
        assert(wss <= ws);
        stat[int(wss*1.0/ws)] += num;
    }
};