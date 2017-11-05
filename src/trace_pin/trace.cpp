#include <pin.H>
#include <ctime>
#include <cstdlib>
#include <map>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <unordered_set>
#include <unordered_map>

typedef UINT32 CACHE_STATS; // type of cache hit/miss counters
#include "pin_cache.H"

#include "lru.h"
#include "compressor.h"

using namespace std;

KNOB<string> KnobOutput(KNOB_MODE_WRITEONCE, "pintool", "o", "SampledTrace", "Output file of trace");
KNOB<UINT64> KnobSample(KNOB_MODE_WRITEONCE, "pintool", "sample", "1000000000", "Number of references sampled(1 billion by default)"); 
KNOB<UINT64> KnobSkip(KNOB_MODE_WRITEONCE, "pintool", "skip", "8192", "reference skipped");
KNOB<INT32> KnobLRUSize(KNOB_MODE_WRITEONCE, "pintool", "lru", "8192", "LRU stack size");

UINT64 sinst = 0;
UINT64 skipped = 0;
UINT64 instcount = 0;
ofstream traceout;
LRUStack *lru;
Compressor *cmp;

map<ADDRINT, int> visited;

LOCALFUN VOID MemRef(ADDRINT addr, UINT32 size, CACHE_BASE::ACCESS_TYPE accessType){
    if(skipped){
        --skipped;
        return;
    }
    if(sinst-- == 0)
        PIN_ExitApplication(0);
    if(lru->access(addr>>6))
        return;
    instcount++;
    //traceout.write((char*)&addr, sizeof(addr));
    cmp->write(addr);
    //visited[addr>>6]++;
    cout << addr << endl;
}

LOCALFUN VOID Instruction(INS ins, VOID *v){
    
    if(INS_IsStandardMemop(ins) && (INS_IsMemoryRead(ins) || INS_IsMemoryWrite(ins))){
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)MemRef,
                INS_IsMemoryRead(ins)?IARG_MEMORYREAD_EA : IARG_MEMORYWRITE_EA,
                INS_IsMemoryRead(ins)?IARG_MEMORYREAD_SIZE : IARG_MEMORYWRITE_SIZE,
                IARG_UINT32, CACHE_BASE::ACCESS_TYPE_LOAD,
                IARG_END);
    }
}


LOCALFUN VOID Fini(int code, VOID * v){
    cmp->close();
    cout << "Finish Sampling" << endl;
    cout << "Instruction Count : " << instcount << endl;
    //traceout.write((char*)&instcount, sizeof(instcount));
    //skipped = KnobSkip.Value();
    //traceout.write((char*)&skipped, sizeof(skipped));
    //traceout.close();
    //cout << visited.size() << endl;
}

LOCALFUN VOID Init(){
    sinst = KnobSample.Value();
    skipped = KnobSkip.Value();
    lru = new LRUStack(KnobLRUSize.Value());
    cmp = new Compressor(KnobOutput.Value().c_str());
    //traceout.open(KnobOutput.Value().c_str(), ofstream::out);
    ///assert(traceout.good());
    cout << "BEGIN TO SAMPLE" << endl;
    cout << "Maximal Inst : " << sinst << endl;
    cout << "LRU Size : " << KnobLRUSize.Value() << endl;
}

GLOBALFUN int main(int argc, char *argv[])
{
    //PIN_InitSymbols();
    PIN_Init(argc, argv);
    Init();
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    PIN_StartProgram();

    return 0; // make compiler happy
}
