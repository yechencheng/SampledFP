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
        if(q.size() == y){            
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

        if(q.size() < y){
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
    double sr;
    dcmp.read(sr);
    double x = y/sr;

    int64_t pos;
    AddrInt addr;
    SampledFootprint sfp(x,y);
    //HeadFrequency hfeq(x,y);
    //WindowFrequency wfeq(x,y);
    size_t nfeq = 0;
    int64_t step = 20*y;
    //int64_t step = 1;
    vector<HeadFrequency> hfeqs;
    vector<WindowFrequency> wfeqs;
    vector<int64_t> nstep;

    for(int64_t i = y; i < 12*y; i += step){
        nfeq++;
        hfeqs.push_back(HeadFrequency(x,i));
        wfeqs.push_back(WindowFrequency(x,i));
        nstep.push_back(i);
    }
    nstep.push_back(x+step);

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
        //if(cnty % 5000000 == 0) cout << cnty << endl;
        //if(cnty  == 50000ll) break;
        //if(cnty == 30) return 0;
        dcmp.read(addr);
        //cout << pos << "\t" << addr << endl;

        //Compute S(X,Y)
        sfp.OnReference(addr,pos);
        for(int i = 0; i < nfeq; i++){
            hfeqs[i].OnReference(addr,pos);
            wfeqs[i].OnReference(addr,pos);
        }
    }
    sfp.Finalize();
    for(int i = 0; i < nfeq; i++){
        hfeqs[i].Finalize();
        wfeqs[i].Finalize();
    }
    
    double g = 0;
    double s = 0;

    vector<pair<double,double>> hfr[nfeq];
    vector<pair<double,double>> wfr[nfeq];
    for(int i = 0; i < nfeq; i++){
        double fsum = 0;
        for(auto j : hfeqs[i].fratio){
            hfr[i].push_back(make_pair(j.first, j.second));
            fsum += j.second;
            //cout << j.first << "\t" << j.second << endl;
        }
        for(auto &j : hfr[i]){
            j.second /= fsum;
            cout << j.first << "\t" << j.second << endl;
        }

        cout << "=================================================================" << endl;


        fsum = 0;
        for(auto j : wfeqs[i].fratio){
            wfr[i].push_back(make_pair(j.first, j.second * j.first));
            fsum += j.second * j.first;
            //cout << j.first/(double)nstep[i] << "\t" << j.second << endl;
        }
        for(auto &j : wfr[i]){
            j.second /= fsum;
            cout << j.first << "\t" << j.second << endl;
        }
        cout << "****************************************************************" << endl;
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
        if(i % 100000 == 0)
            cout << g << "\t" << s << endl;
    }

    double delta = 0;
    for(int64_t i = 10*y; i < 12*y; i++){
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

    delta = 100;
    cout << "Predicted fp(y) : " << sfp.dfp-g+s-delta*(x-10*y)/(2*y) << endl;
    cout << "s(x,y) : " << sfp.dfp << endl;
    cout << "g(x,y) : " << g << endl;
    cout << "s(x,y)-s(x,y-1) : " << s << endl;
    cout << "g-s : " << g-s << endl;
    cout << "delta : " << delta << endl;
    return 0;
}