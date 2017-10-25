#include "lru.h"

#include <iostream>
using namespace std;

LRUStack::LRUStack(int _size) : size(_size){
    data = new AddrInt[size];
    prev = new int[size];
    next = new int[size];
    pbottom = 0;
    ptop = 0;
}

bool LRUStack::contain(AddrInt address){
    if(sblock.find(address) == sblock.end())
        return false;
    int pos = sblock[address];
    return data[pos] == address;
}

AddrInt LRUStack::bottom(){
    return data[pbottom];
}

bool LRUStack::access(AddrInt address){
    bool is_contain = contain(address);
    if(is_contain){
        move_to_top(sblock[address]);
        return true;
    }

    // insert new data block if cache is not full
    if(sblock.size() < size){
        int new_pbottom = sblock.size();
        next[pbottom] = new_pbottom;
        prev[new_pbottom] = pbottom;
        pbottom = new_pbottom;
    }

    data[pbottom] = address;
    sblock[address] = pbottom;
    move_to_top(pbottom);
    return false;
}

void LRUStack::move_to_top(int pos){
    if(pos == ptop) return;

    int new_pbottom = (pos == pbottom ? prev[pos] : pbottom);
    int new_top = pos;

    next[prev[pos]] = next[pos];
    if(pos != pbottom)
        prev[next[pos]] = prev[pos];
    
    prev[pos] = -1;
    next[pos] = ptop;
    prev[ptop] = pos;

    pbottom = new_pbottom;
    ptop = new_top;
}