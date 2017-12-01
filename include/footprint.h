#pragma once
#include <iostream>
#include <fstream>
#include <cinttypes>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <cassert>

#include "hist.h"
using namespace std;

const uint64_t RTMax = (1ull) << 63;

template<typename HistType = HashHist<uint64_t, uint64_t>>
class FPBuilderBase;

class FPDist{
    template<typename HistType>
    friend class FPBuilderBase;
protected:
    uint64_t naccess;
    uint64_t ndata;
public:
    HashHist<double> FPHist;
    FPDist();
    uint64_t GetNData();
    uint64_t GetNAccesses();
    double operator[](double x);
    double GetNMisses(double ft);
    void serialize(ostream &out);
    void serialize(string file);
    static FPDist* deserialize(istream &in);
    static FPDist* deserialize(string file);
};

template<typename HistType>
class FPBuilderBase{
protected:
    HistType rthist; //<! histogram of reuse time
    unordered_map<uint64_t, uint64_t> firstref; //<! temporary data used while building FPHist
    unordered_map<uint64_t, uint64_t> lastref;  //<! temporary data used while building FPHist

    uint64_t naccess;
    uint64_t ndata;
public:
    HashHist<double> FPHist;
public:
    FPBuilderBase();
    void Start();
    virtual void OnReference(uint64_t a); //<! accesses referece a data
    virtual FPDist* Finalize(); //<! finalize the process
    virtual FPDist* ComposeFPDist();
};

template<typename HistType = HashHist<uint64_t, uint64_t>>
class PreciseFPBuilder : public FPBuilderBase<HistType>{
public:
    using FPBuilderBase<HistType>::naccess;
    using FPBuilderBase<HistType>::ndata;
    using FPBuilderBase<HistType>::rthist;
    using FPBuilderBase<HistType>::firstref;
    using FPBuilderBase<HistType>::lastref;
    using FPBuilderBase<HistType>::FPHist;
    HistType arthist;
public:
    void OnReference(uint64_t a);
    FPDist* Finalize();
    using FPBuilderBase<HistType>::ComposeFPDist;
};


class VFPDist : public FPDist{};
typedef PreciseFPBuilder<> FPBuilder;
//typedef FPBuilderBase<> FPBuilder;



void FPDist::serialize(string file){
    ofstream fout(file.c_str());
    assert(fout.good());
    serialize(fout);
    fout.close();
}

void FPDist::serialize(ostream &out){
    Write(out, naccess);
    Write(out, ndata);
    FPHist.serialize(out);
}

FPDist* FPDist::deserialize(string file){
    ifstream fin(file.c_str());
    assert(fin.good());
    auto rt = FPDist::deserialize(fin);
    fin.close();
    return rt;
}

FPDist* FPDist::deserialize(istream &in){
    FPDist* rt = new FPDist();
    Read(in, rt->naccess);
    Read(in, rt->ndata);
    auto x = HashHist<double>::deserialize(in);
    rt->FPHist = *x;
    return rt;
}

double FPDist::operator[](double x){
    if(x >= naccess)
        return ndata;
    return FPHist.GetSmoothedYValue(x);
    //return FPHist[x];
}

FPDist::FPDist(){
    naccess = 0;
    ndata = 0;
}

uint64_t FPDist::GetNData(){
    return ndata;
}

uint64_t FPDist::GetNAccesses(){
    return naccess;
}

template<typename HistType>
void FPBuilderBase<HistType>::Start(){
    FPHist.Clear();
    naccess = 0;
    ndata = 0;
}

template<typename HistType>
FPBuilderBase<HistType>::FPBuilderBase(){
    Start();
}

template<typename HistType>
FPDist* FPBuilderBase<HistType>::ComposeFPDist(){
    FPDist* rt = new FPDist();
    rt->naccess = naccess;
    rt->ndata = ndata;
    rt->FPHist = FPHist;
    return rt;
}

template<typename HistType>
void FPBuilderBase<HistType>::OnReference(uint64_t a){
    naccess++;
    if(firstref.find(a) == firstref.end()){
        firstref[a] = naccess;
    }
    else{
        uint64_t rtime = naccess - lastref[a];
        rthist[rtime]++;
    }
    lastref[a] = naccess;
}

template<typename HistType>
FPDist* FPBuilderBase<HistType>::Finalize(){
    ndata = firstref.size();

    auto W = FPHist.GetXValues();
    //<! counting f_i an l_i
    vector<uint64_t> fl;  //<! first access times
    for(auto i : firstref)
        fl.push_back(i.second);
    for(auto i : lastref)
        fl.push_back(naccess+1-i.second);
    sort(fl.begin(), fl.end());

    vector<uint64_t> ta(W.size(), 0);
    vector<uint64_t> tb(W.size(), 0);

    for(size_t i = 0; i < W.size(); i++){
        ta[i] = W[i]*rthist[W[i]];
        tb[i] = rthist[W[i]];
    }

    if(W.size() >= 2){
        for(int i = W.size()-2; i >= 0; i--){
            ta[i] += ta[i+1];
            tb[i] += tb[i+1];
        }
    }

    for(auto w : W){
        if(w >= naccess){
            FPHist[w] = ndata;
            continue;
        }
        uint64_t fp = 0;
        for(int i = fl.size()-1; i >= 0; i--){
            if(fl[i] <= w) break;
            fp += fl[i]-w;
        }

        /*
        for(int i = W.size(); i >= 0; i--){
            if(W[i] <= w) break;;
            //fp += naccess*(W[i]-w)*rthist[W[i]];
            fp += (W[i]-w)*rthist[W[i]];
        }
        */

        int idx = upper_bound(W.begin(), W.end(), w) - W.begin();
        fp += ta[idx] - tb[idx]*w;

        fp = ndata * (naccess-w+1) - fp;

        double fp_double = fp / (double)(naccess-w+1);
        FPHist[w] = fp_double;
    }
    return ComposeFPDist();
}



template<typename HistType>
void PreciseFPBuilder<HistType>::OnReference(uint64_t a){
    naccess++;
    if(firstref.find(a) == firstref.end()){
        firstref[a] = naccess;
    }
    else{
        uint64_t rtime = naccess - lastref[a];
        rthist[rtime]++;
        arthist[rtime] += rtime;
    }
    lastref[a] = naccess;
}

template<typename HistType>
FPDist* PreciseFPBuilder<HistType>::Finalize(){
    ndata = firstref.size();

    //<! counting f_i an l_i
    vector<uint64_t> fl;  //<! first access times
    for(auto i : firstref)
        fl.push_back(i.second);
    for(auto i : lastref)
        fl.push_back(naccess+1 - i.second);
    sort(fl.begin(), fl.end());
    vector<uint64_t> afl = fl;
    for(int i = fl.size()-2; i >= 0; i--)
        afl[i] += afl[i+1];

    auto W = FPHist.GetXValues();
    for(int i = W.size()-2; i >= 0; i--){
        rthist[W[i]] += rthist[W[i+1]];
        arthist[W[i]] += arthist[W[i+1]];
    }

    for(size_t i = 0, pos = 0; i+1 < W.size(); i++){
        if(W[i] >= naccess){
            FPHist[W[i]] = ndata;
            continue;
        }
        while(pos < fl.size() && fl[pos] <= W[i]) pos++;

        //sum(f(i))
        uint64_t fp = pos == afl.size() ? 0 : afl[pos];
        //W[i]*(num of rt(i) >= W[i])
        fp -= (afl.size()-pos) * W[i];

        //t*r(t) - w*r(t)
        fp += arthist[W[i]];
        fp -= W[i] * rthist[W[i]];

        fp = ndata*(naccess-W[i]+1) - fp;

        FPHist[W[i]] = (fp/(double)(naccess-W[i]+1));
    }
    for(int i = 0; i < W.size()-1; i++)
        rthist[W[i]] -= rthist[W[i+1]];
    return ComposeFPDist();
}
