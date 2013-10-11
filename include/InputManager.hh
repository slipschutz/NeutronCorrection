
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <string>
#include "TTree.h"
#include <map>
using namespace std;

class InputManager {

public:
  
  InputManager();
  ~InputManager();
  
  Bool_t loadInputs(vector <string> &);
  Bool_t loadInputs2(vector <string> &);


  Int_t runNum;
  Double_t numFiles;

  Bool_t fast;
  Bool_t lean;
  Double_t traceDelay;

  Bool_t ext_flag;
  Bool_t reMakePulseShape;
  string timingMode;
  Double_t FL,FG,d,w;

  Double_t short_gate,long_gate;

  string specificFileName;

  Double_t sigma;

  Bool_t ext_sigma_flag;
private:
  vector <string> split (const string &s, char delim, vector<string> &elems);
  vector <string> split (const string &s, char delim);

  Bool_t checkValues();
  
  string lowerCase(string);

  map < string , Double_t *> ValidNumericalInputs; 
  map < string , Bool_t * > ValidBoolInputs;
  map < string , string* > ValidStringInputs;
  void BuildInputMap();

  vector<string> validTimingModes;
  void dumpValidModes();

  void DumpAllOpitions();

  void PrintValues();

};
