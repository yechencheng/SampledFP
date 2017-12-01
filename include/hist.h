#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <iostream>

#include "general.hpp"

using namespace std;

/**histogram with yvalues as YType and xvalues as XType
 !*/
template<typename YType, typename XType>
class Hist{
public:
    vector<XType> xval;
    vector<YType> yval;
public:
    Hist();
    virtual size_t GetXID(XType rawval); //<! find the closet XID for xval
    virtual YType& operator[](XType rawval); //<! given x value, return y value
    virtual vector<XType> GetXValues(); //<! return all x values
    virtual vector<YType> GetYValues(); //<! return all y values
    YType& GetYValue(XType rawval); //<! given x, return y
    YType GetSmoothedYValue(XType rawid); //<! smooth the histogram, given x return y.

    XType GetXValue(YType rawval); //<! given y, return related x
    XType GetSmoothedXValue(YType rawid); //<! smooth the histogram, given y return x.
    virtual void serialize(ostream &out);
    static Hist<YType, XType>* deserialize(istream &in);
    void Clear();
    virtual void Init();
};

template<typename YType, typename XType>
void Hist<YType, XType>::serialize(ostream &out){
    size_t sz = yval.size();
    Write(out, sz);
    for(size_t i = 0; i < sz; i++)
        Write(out, xval[i]);
    for(size_t i = 0; i < sz; i++)
        Write(out, yval[i]);
}


template<typename YType, typename XType>
Hist<YType, XType>* Hist<YType, XType>::deserialize(istream &in){
    Hist<YType, XType> *rt = new Hist<YType, XType>();
    rt->xval.clear();
    rt->yval.clear();
    size_t sz;
    Read(in, sz);
    for(uint64_t i = 0; i < sz; i++){
        XType x;
        Read(in, x);
        rt->xval.push_back(x);
    }
    for(uint64_t i = 0; i < sz; i++){
        YType y;
        Read(in, y);
        rt->yval.push_back(y);
    }
    return rt;
}

template<typename YType, typename XType>
void Hist<YType, XType>::Init(){
    if(xval.size() == 0)
        GetXValues();
}

template<typename YType, typename XType>
Hist<YType,XType>::Hist(){
    Init();
}

template<typename YType, typename XType>
void Hist<YType, XType>::Clear(){
    xval.clear();
    yval.clear();
    Init();
}

template<typename YType, typename XType>
size_t Hist<YType, XType>::GetXID(XType rawval){
    size_t xid = lower_bound(xval.begin(), xval.end(), rawval) - xval.begin();
    return min(xid, xval.size()-1);
}

template<typename YType, typename XType>
YType& Hist<YType,XType>::operator[](XType rawval){
    return GetYValue(rawval);
}

template<typename YType, typename XType>
vector<YType> Hist<YType, XType>::GetYValues(){
    return yval;
}

template<typename YType, typename XType>
vector<XType> Hist<YType, XType>::GetXValues(){
    return xval;
}

template<typename YType, typename XType>
XType Hist<YType, XType>::GetXValue(YType rawval){
    size_t xid = lower_bound(yval.begin(), yval.end(), rawval) - yval.begin();
    xid = min(xid, yval.size()-1);
    return xval[xid];
}

template<typename YType, typename XType>
YType& Hist<YType, XType>::GetYValue(XType rawval){
    return yval[GetXID(rawval)];
}


template<typename YType, typename XType>
XType Hist<YType, XType>::GetSmoothedXValue(YType rawval){
    size_t xid = lower_bound(yval.begin(), yval.end(), rawval) - yval.begin();
    xid = min(xid, yval.size()-1);
    if(xid+1 == yval.size()) return xval[xid];
    XType rt = (rawval-yval[xid])*(xval[xid+1]-xval[xid]);
    rt /= (yval[xid+1]-yval[xid]);
    rt += xval[xid];
    return rt;
}

template<typename YType, typename XType>
YType Hist<YType, XType>::GetSmoothedYValue(XType rawval){
    if(xval.size() == 0) GetXValues();
    size_t xid = GetXID(rawval);
    if(xid+1 == xval.size()) return yval[xid];

    YType rt = rawval-xval[xid];
    if(yval[xid+1] >= yval[xid]){
        rt *= (yval[xid+1]-yval[xid]);
        rt /= (xval[xid+1]-xval[xid]);
        rt += yval[xid];
    }
    else{
        rt *= yval[xid]-yval[xid+1];
        rt /= xval[xid+1]-xval[xid];
        rt = yval[xid]-rt;
    }
    return rt;
}


template<typename YType = uint64_t, typename XType = uint64_t>
class HashHist : public Hist<YType,XType>{
private:
public:
    int SUBLOG_BITS; //<! keep the highest SUBLOG_BITS bits

    using Hist<YType, XType>::yval;
    using Hist<YType, XType>::xval;

    HashHist(int S = 6);
    size_t GetXID(XType rawid) override;
    vector<XType> GetXValues() override;
    vector<YType> GetYValues() override;

    void serialize(ostream &out) override;
    void Init() override;
    static HashHist<YType, XType>* deserialize(istream& in);
};

template<typename YType, typename XType>
void HashHist<YType, XType>::serialize(ostream &out){
    Write(out, SUBLOG_BITS);
    Write(out, yval.size());
    for(auto i : yval)
        Write(out, i);
}

template<typename YType, typename XType>
HashHist<YType, XType>* HashHist<YType, XType>::deserialize(istream &in){
    HashHist<YType, XType>* rt = new HashHist<YType, XType>();
    rt->xval.clear();
    rt->yval.clear();
    Read(in, rt->SUBLOG_BITS);
    size_t sz;
    Read(in, sz);
    for(size_t i = 0; i < sz; i++){
        YType y;
        Read(in, y);
        rt->yval.push_back(y);
    }
    return rt;
}

template<typename YType, typename XType>
void HashHist<YType, XType>::Init(){
    yval = vector<YType>((1<<SUBLOG_BITS)*64,0);
}

template<typename YType, typename XType>
HashHist<YType,XType>::HashHist(int S){
    SUBLOG_BITS = S;
    Init();
}

template<typename YType, typename XType>
size_t HashHist<YType,XType>::GetXID(XType rawid){
	if (rawid < ((XType)1<<SUBLOG_BITS))
	    return rawid;

	size_t index;
	size_t msb, shift;
	msb = 63 - __builtin_clzll(rawid);
	shift = msb - SUBLOG_BITS;
	index = rawid >> shift;
	index &= (1<<SUBLOG_BITS) - 1;
	index = ((shift + 1) << SUBLOG_BITS) + index;
	return index;
}


template<typename YType, typename XType>
vector<XType> HashHist<YType, XType>::GetXValues(){
    if(xval.size()) return xval;
    uint64_t upper = uint64_t(1) << SUBLOG_BITS;
    for(uint64_t i = 0; i < upper; i++)
        xval.push_back(i);
    for(uint64_t i = upper; i != 0; i <<= 1){
        for(uint64_t j = 0; j < i; j += i>>SUBLOG_BITS)
            xval.push_back(i+j);
    }
    return xval;
}

template<typename YType, typename XType>
vector<YType> HashHist<YType, XType>::GetYValues(){
    size_t s = GetXValues().size(); //technical reason, y value will be greater than x value
    yval.resize(s);
    return yval;
}
