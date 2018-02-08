#include <iostream>
#include <fstream>
#include <map>
#include <unordered_set>
#include <queue>
#include <algorithm>

#include <boost/program_options.hpp>
#include <boost/circular_buffer.hpp>

#include "statistic.h"
#include "compressor.h"
#include "type.h"
#include "sampler.h"
#include "hist.h"

using namespace std;
using namespace boost::program_options;

vector<int64_t> GenerateYList(){
    vector<int64_t> rt;
    //for(int64_t i = 65536; i < 786432; i += 512)
    for(int64_t i = 65536; i < 786432; i += 4096)
        rt.push_back(i);
    //rt.push_back(65536);
    return rt;
}

void test(){

    vector<AddrInt> trace;
    trace.push_back(0);
    trace.push_back(0);
    trace.push_back(1);
    trace.push_back(0);
    trace.push_back(0);
    trace.push_back(1);
    trace.push_back(0);
    trace.push_back(0);
    trace.push_back(1);
    trace.push_back(0);

    WindowFrequency wfq(100, 4);
    HeadFrequency hfq(100,2);
    for(auto i : trace){
        wfq.OnReference(i,0);
        hfq.OnReference(i,0);
    }
    wfq.Finalize();
    hfq.Finalize();
    wfq.output();
    cout << string('=',20) << endl;
    hfq.output();


}

int main(int argc, char** argv){
    string fname;
    double y;

    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "This message")
        ("trace,t", value<string>(&fname)->required(), "input trace file")
        ("yval,y", value<double>(&y)->required(), "value of y, using s(x,y) predict f(y)")
    ;
    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    if(vm.count("help")){
        cerr << desc << endl;
        return 1;
    }
    notify(vm);

    Decompressor dcmp(fname);
    double sr=1.0;
    dcmp.read(sr);
    double x = y/sr;
    cout << "x,y = " << x << " " << y << endl;
    int64_t pos;
    AddrInt addr;
    SampledFootprint sfp(x,y);
    //HeadFrequency hfeq(x,y);
    //WindowFrequency wfeq(x,y);
    size_t nfeq = 0;
    int64_t step = 655360;
    //int64_t step = 1;
    vector<HeadFrequency> hfeqs;
    vector<WindowFrequency> wfeqs;
    vector<int64_t> nstep;

    for(int64_t i = y; i < 20*y; i += step){
        nfeq++;
        hfeqs.push_back(HeadFrequency(x,i));
        wfeqs.push_back(WindowFrequency(x,i));
        nstep.push_back(i);
    }
    nstep.push_back(x+step);
    cout << "num of frequency distribution : " << nstep.size() << endl;

    /*
    nfeq++;
    hfeqs.push_back(HeadFrequency(x,y));
    wfeqs.push_back(WindowFrequency(x,y));
    nstep.push_back(y);
    nfeq++;
    hfeqs.push_back(HeadFrequency(x,100*y));
    wfeqs.push_back(WindowFrequency(x,100*y));
    nstep.push_back(y);
    */

    int64_t cnty = 0;
    
    while(dcmp.read(pos)){
        cnty++;
        if(cnty % 5000000 == 0) cout << cnty << endl;
        //if(cnty  / y == 1000) break;
        //if(cnty == 30) return 0;
        dcmp.read(addr);
        //cout << pos << "\t" << addr << endl;

        //Compute S(X,Y)
        sfp.OnReference(addr,pos);
        for(size_t i = 0; i < nfeq; i++){
            hfeqs[i].OnReference(addr,pos);
            wfeqs[i].OnReference(addr,pos);
        }
    }
    sfp.Finalize();
    for(size_t i = 0; i < nfeq; i++){
        hfeqs[i].Finalize();
        wfeqs[i].Finalize();
    }
    
    double g = 0;
    double s = 0;

    vector<pair<double,double>> hfr[nfeq];
    vector<pair<double,double>> wfr[nfeq];
    for(size_t i = 0; i < nfeq; i++){
        hfeqs[i].output();
        cout << string('=', 20) << endl;
        wfeqs[i].output();
        cout << string('*', 20) << endl;
        
        double fsum = 0;
        for(auto j : hfeqs[i].fratio){
            hfr[i].push_back(make_pair(j.first, j.second));
            fsum += j.second;
        }
        for(auto &j : hfr[i]){
            j.second /= fsum;
        }

        fsum = 0;
        for(auto j : wfeqs[i].fratio){
            wfr[i].push_back(make_pair(j.first, j.second * j.first));
            fsum += j.second * j.first;
        }
        for(auto &j : wfr[i]){
            j.second /= fsum;
        }
    }
    
    int hfr_size = 2;
    int wfr_size = 2;

    pos = 0;
    for(int64_t i = y+1; i < 10*y; i++){
        if(nstep[pos+1] <= i) {pos++;}
        hfr_size = hfr[pos].size();
        wfr_size = wfr[pos].size();
        int hpos = pos;
        int wpos = pos;
        //double tg = 0;
        for(int j = 0; j < hfr_size; j++){
            if(hfr[hpos][j].first > i-y+1) continue;
            g += y/(double)i*pow((i-y)/(i-1),j)*hfr[hpos][j].second;
            //tg += y/(double)i*pow((i-y)/(i-1),j)*hfr[pos][j].second;
        }
        //cout << i << "\t" << tg << endl;
        for(int j = 0; j < wfr_size; j++){
            if(wfr[wpos][j].first > i-y+1) continue;
            s += y/(double)i*pow((i-y)/(i-1),j)*wfr[wpos][j].second;
        }
        //if(i % 100000 == 0)
            //cout << g << "\t" << s << endl;
    }

    double delta = 0;
    for(int64_t i = 10*y; i < 20*y; i++){
        if(nstep[pos+1] <= i) {pos++;}
        hfr_size = hfr[pos].size();
        wfr_size = wfr[pos].size();
        int hpos = pos;
        int wpos = pos;
        for(int j = 0; j < hfr_size; j++){
            if(hfr[hpos][j].first > i-y+1) continue;
            delta += y/(double)i*pow((i-y)/(i-1),j)*hfr[hpos][j].second;
        }
        for(int j = 0; j < wfr_size; j++){
            if(wfr[wpos][j].first > i-y+1) continue;
            delta -= y/(double)i*pow((i-y)/(i-1),j)*wfr[wpos][j].second;
        }
    }
    //delta = 100;
    //double sdelta = delta*(x-10*y)/(20*y);
    //double sdelta = delta * log(10)/log(2);
    double sdelta = delta*3.321928;
    sdelta = delta;
    cout << "Predicted fp(y) : " << sfp.dfp-g+s-sdelta << endl;
    cout << "s(x,y) : " << sfp.dfp << endl;
    
    cout << "g-s : " << g-s << endl;
    cout << "sdelta : " << sdelta << endl;
    
    cout << "g(x,y) : " << g << endl;
    cout << "s(x,y)-s(x,y-1) : " << s << endl;
    cout << "delta : " << delta << endl;

    cout << endl;
    
    return 0;
}
