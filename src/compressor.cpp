#include <fstream>
#include <string>
#include <cassert>
#include <iostream>

#include "compressor.h"

#include <lz4.h>

using namespace std;

Compressor::Compressor(string outfile, int _buff_size) : buff_size(_buff_size){
    current_pos = 0;
    fout.open(outfile, ofstream::binary);
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

Decompressor::Decompressor(string infile, int _buff_size) : buff_size(_buff_size){
    current_pos = 0;
    data_size = 0;
    fin.open(infile, ifstream::binary);
    assert(fin.good());
    buff = new char[buff_size*2];
    dcmp = new char[buff_size*2];
}



bool Decompressor::fill_buffer(){
    memcpy(buff, buff+current_pos, data_size-current_pos);
    current_pos = data_size-current_pos;
    if(!fin.read((char*)&data_size, sizeof(data_size))){
        assert(current_pos == 0);
        return false;
    }
    fin.read(dcmp, data_size);
    data_size = LZ4_decompress_safe(dcmp, buff+current_pos, data_size, buff_size);
    assert(data_size > 0);
    data_size += current_pos;
    current_pos = 0;
    return true;
}

void Decompressor::close(){
    if(!fin.good()) return;
    fin.close();
}

Decompressor::~Decompressor(){
    close();
    delete buff;
}