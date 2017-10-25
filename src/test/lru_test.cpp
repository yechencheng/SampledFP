#include <iostream>
#include "lru.h"

using namespace std;

int main(){
    LRUStack s(3);
    AddrInt seq[] = {1,2,3,4,2,3,1,4,4,4,3,4,2,1};
    int len = sizeof(seq)/sizeof(AddrInt);
    for(int i = 0; i < len; i++)
        cout << seq[i] << " " << s.access(seq[i]) << endl;
    return 0;
}