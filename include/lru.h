#pragma once

#include <unordered_map>

#include "type.h"

using namespace std;

//Fully associated LRU
class LRUStack{
private:
    size_t size;
    unordered_map<AddrInt, int> sblock;
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
