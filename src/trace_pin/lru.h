#include <unordered_map>
#include <map>
#include "type.h"

using namespace std;

//Fully associated LRU
class LRUStack{
private:
    size_t size;
    tr1::unordered_map<AddrInt, int> sblock;
    AddrInt *data;
    int *prev;
    int *next;
    int pbottom; 
    int ptop;

    void move_to_top(int pos);
public:
    LRUStack(int _size);
    bool contain(AddrInt address); //return if the data block is contained in the LRU
    AddrInt bottom(); //return the data block to be evicted
    bool access(AddrInt address); //return true if the data block is contained in the LRU stack
};


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