#pragma once

#include <string>
#include <fstream>

using namespace std;

static const int default_buff_size = 512<<20;
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

class Decompressor{
private:
    ifstream fin;
    int buff_size;
    char *buff;
    int data_size;

    char* dcmp;
    int current_pos;
    bool fill_buffer();
public:
    Decompressor(string infile, int _buff_size=default_buff_size);

    template<typename T> bool read(T &a);
    void close();
    ~Decompressor();
};

template<typename T> 
bool Compressor::write(T a){
    if(current_pos + sizeof(a) >= buff_size)
        clear_buffer();
    *(T*)(buff+current_pos) = a;
    current_pos += sizeof(a);
    return true;
}

template<typename T>
bool Decompressor::read(T &a){
    if(current_pos + sizeof(a) > data_size){
        if(!fill_buffer())  //no more data
            return false;
    }
    a = *(T*)(buff+current_pos);
    current_pos += sizeof(a);
    return true;
}
