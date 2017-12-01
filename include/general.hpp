#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

#include "type.h"

using namespace std;

vector<vector<int>> getAllComb(int N, int K);

template<typename T>
void Read(istream &is, T &x){
        is.read((char*)&x, sizeof(T));
}

template<typename T>
void Write(ostream &os, T x){
        os.write((char*)&x, sizeof(T));
}
