#include <iostream>
#include <string>
#include <cassert>

#include "compressor.h"
#include "type.h"

using namespace std;

int main(){
    int a = 0;
    int64_t b = 0;

    int buff_size = 24;
    Compressor cmp("cmp_test", buff_size);
    
    for(int i = 0; i < 100; i++){
        cmp.write(a++);
        cmp.write(b++);
        cmp.write(a++);
    }
    cmp.close();

    Decompressor dcmp("cmp_test", buff_size);
    for(int i = 0; i < 100; i++){
        dcmp.read(a);
        assert(a == i*2);
        dcmp.read(b);
        assert(b == i);
        dcmp.read(a);
        assert(a == i*2+1);
    }
    dcmp.close();

    return 0;
}