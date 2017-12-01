#pragma once

#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>
#include <cassert>
#include "footprint.h"

void NormalizeAR(vector<double> &ar){
    if(ar[0] < 1.0) return;

    double x = 0;
    accumulate(ar.begin(), ar.end(), x);
    for(auto &i : ar)
        i /= x;
}

class FPUtil{
public:
	static double GetMissRatio(FPDist *fp, double c);
    static double GetMissRatio(FPDist *fp, HashHist<uint64_t> *rthist, double c);
	//static double GetMissRatioColdMiss(FPDist *fp, double c);
	static double GetFP(FPDist *fp, double w);
	static double GetFT(FPDist *fp, double c);
	static double GetNData(FPDist *fp);
    static double GetNAccesses(FPDist *fp);
	static void serialize(FPDist *fp, string filename = "footprint");
	static FPDist* deserialize(string filename = "footprint");
};


class FPManager{
private:
    FPDist *fp;
public:
    double GetMissRatio(double c){return FPUtil::GetMissRatio(fp, c);}
    double GetFP(double w){return FPUtil::GetFP(fp, w);}
    double GetFT(double c){return FPUtil::GetFT(fp, c);}
    double GetNData(){return FPUtil::GetNData(fp);}
    void serialize(string filename = "footprint"){FPUtil::serialize(fp, filename);}
    void deserialize(string filename = "footprint"){fp = FPUtil::deserialize(filename);}
    FPDist* GetFPDist() {return fp;}
};

double FPUtil::GetMissRatio(FPDist *fp, double c){
	if(c == 0) return 1;
	double tc = GetFT(fp, c);
	return GetFP(fp, tc+1)-GetFP(fp, tc);
    
}

double FPUtil::GetMissRatio(FPDist *fp, HashHist<uint64_t> *rthist, double c){
	uint64_t tc = GetFT(fp, c);
    //Count RTHist < tc
    auto xval = rthist->GetXValues();
    auto yval = rthist->GetYValues();
    int idx = upper_bound(xval.begin(), xval.end(), tc) - xval.begin();
    uint64_t nmisses = 0;
    nmisses = accumulate(yval.begin(), yval.begin()+idx, (uint64_t)0);
    return 1-nmisses/(double)fp->GetNAccesses();
}

double FPUtil::GetFP(FPDist *fp, double w){
	return (*fp)[w];
}

//<! ft(c) = (c-b)/(a-b)*[ft(a)-ft(b)] + ft(b)
double FPUtil::GetFT(FPDist *fp, double c){
    if(c >= fp->GetNData())
        return fp->GetNAccesses();
    uint64_t l = 0, h = fp->GetNAccesses();
    while(l < h){
        uint64_t mid = (l+h)/2;
        if(GetFP(fp,mid) < c)
            l = mid+1;
        else
            h = mid;
    }
    return l;
    //return fp->FPHist.GetXValue(c);
}

double FPUtil::GetNData(FPDist *fp){
	return fp->GetNData();
}

double FPUtil::GetNAccesses(FPDist *fp){
    return fp->GetNAccesses();
}

void FPUtil::serialize(FPDist *fp, string filename){
	ofstream fout;
	fout.open(filename.c_str());
    fp->serialize(fout);
	fout.close();
}

FPDist* FPUtil::deserialize(string filename){
	ifstream fin(filename.c_str());
	assert(fin.good());
    FPDist *_fp = new FPDist();
    _fp->deserialize(fin);
	fin.close();
    return _fp;
}

//<! ar should be normalized, input fill time w, return composed fill time, normalized to access rate ar.
double ComposedFP(double w, vector<FPManager*> fps, vector<double> &ar){
    NormalizeAR(ar);
    double rt = 0;
    for(size_t i = 0; i < fps.size(); i++)
        rt += fps[i]->GetFP(w*ar[i]);
    return rt;
}

//<! input cach size(c), return composed fill times of fps, ar can be unormalized.
double ComposedFT(double c, vector<FPManager*> fps, vector<double> &ar){
    NormalizeAR(ar);
    double l = 0, h = 1e20;
    while(h-l > 1e-6){
        double mid = (l+h)/2;
        double mc = ComposedFP(mid, fps, ar);
        if(mc < c) l = mid;
        else h = mid;
    }
    return l;
}
