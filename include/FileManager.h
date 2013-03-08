//
//  File managerment class
//

#include "TFile.h"
#include <sstream>
#include <string>
using namespace std;
class FileManager {

public:
  FileManager();
  TString loadFile(Int_t runNum,Int_t );

  TFile * getOutputFile();
  TFile * getOutputFile(Double_t FL, Double_t FG, Double_t d, Double_t w);
  TFile * getOutputFile(Double_t sigma);

  TFile *getOutputFile(string );

  //File name streams
  std::stringstream fileName;
  std::stringstream outputFileName;


  string timingMode;
  

};
