#include "footprint.h"
#include "footprintManager.h"
#include "hist.h"

using namespace std;

int main(int argc, char** argv){
    FPDist &fp = *(FPDist::deserialize(argv[1]));
    auto xval = fp.FPHist.GetXValues();
    auto yval = fp.FPHist.GetYValues();
    
    for(size_t i = 0; i < xval.size(); i++)
        cout << xval[i] << "\t" << yval[i] << endl;
    return 0;
}
