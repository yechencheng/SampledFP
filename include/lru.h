#pragma once

#include <map>

#include "type.h"

using namespace std;

//Fully associated LRU
class LRUStack{
private:
    uint64_t size;
    unordered_map<AddrInt> sblock;
public:
    LRUStack(uint64_t _size);
    bool contain(AddrInt address); //return if the data block is contained in the LRU
    AddrInt bottom(); //return the data block to be evicted
    bool access(AddrInt address); //return true if the data block is contained in the LRU stack
};