#include "footprint.h"
#include "footprintManager.h"
#include "hist.h"

using namespace std;

int main(int argc, char** argv){
    cout << "usage : command fp_file" << endl;
    FPDist &fp = *(FPDist::deserialize(argv[1]));
    auto xval = fp.FPHist.GetXValues();
    auto yval = fp.FPHist.GetYValues();
    
    for(int64_t i = 65536; i < 786432; i += 4096)
        cout << i << "\t" << fp.FPHist[i] << "\t" << FPUtil::GetMissRatio(&fp, fp.FPHist[i]) << endl;
    //for(int i = 0; i < xval.size(); i++)
    //    cout << xval[i] << "\t" << yval[i] << endl;
    return 0;
}
