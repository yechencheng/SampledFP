#pragma once

#include <fstream>
#include <string>
#include <cassert>
#include <iostream>

#include "lz4.h"
using namespace std;

static const int default_buff_size = 4<<20;
class Compressor{
private:
    ofstream fout;
    int buff_size;
    char *buff;
    
    char *cmp;
    int current_pos;
    bool clear_buffer();
public:
    Compressor(string outfile, int _buff_size=default_buff_size);

    template<typename T> bool write(T a);
    void close();
    ~Compressor();
};

template<typename T> 
bool Compressor::write(T a){
    if(current_pos + sizeof(a) >= buff_size)
        clear_buffer();
    *(T*)(buff+current_pos) = a;
    current_pos += sizeof(a);
    return true;
}

Compressor::Compressor(string outfile, int _buff_size) : buff_size(_buff_size){
    current_pos = 0;
    fout.open(outfile.c_str(), ofstream::binary);
    assert(fout.good());
    buff = new char[buff_size*2];
    cmp = new char[buff_size*2];
}



bool Compressor::clear_buffer(){
    const int max_dst_size = LZ4_compressBound(current_pos);
    const int cmp_data_size = LZ4_compress_default(buff, cmp, current_pos, max_dst_size);
    assert(cmp_data_size > 0);
    fout.write((char*)&cmp_data_size, sizeof(cmp_data_size));
    fout.write(cmp, cmp_data_size);
    current_pos = 0;
    return true;
}

void Compressor::close(){
    if(!fout.good()) return;
    if(current_pos > 0)
        clear_buffer();
    fout.close();
}

Compressor::~Compressor(){
    close();
    delete buff;
    delete cmp;
}
