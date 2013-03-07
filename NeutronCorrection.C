#include <stdio.h>
#include <stdlib.h>

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include <TRandom1.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include "TRandom3.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TGraph.h"
#include "TChain.h"

//Local Headers
#include "LendaEvent.hh"
#include "Filter.hh"
#include "FileManager.h"
#include "InputManager.hh"
#include "CorrectionManager.hh"



#include "functions.hh"
#define BAD_NUM -10008



using namespace std;


int main(int argc, char **argv){

  vector <string> inputs;
  for (int i=1;i<argc;++i){
    inputs.push_back(string(argv[i]));
  }
  if (inputs.size() == 0 ){ // no argumnets display helpful message
    cout<<"Usage: ./NeutronBuilder runNumber [options:value]"<<endl;
    return 0;
  }  
  
  InputManager theInputManager;
  if ( !  theInputManager.loadInputs(inputs) ){
    return 0;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////



  //load correcctions and settings
  
  Double_t sigma=theInputManager.sigma; // the sigma used in the fitting option

  Int_t runNum=theInputManager.runNum;
  Int_t numFiles=theInputManager.numFiles;

  Long64_t maxentry=-1;

  Bool_t makeTraces=theInputManager.makeTraces;

  Bool_t extFlag=theInputManager.ext_flag;
  Bool_t ext_sigma_flag=theInputManager.ext_sigma_flag;

  //defualt Filter settings see pixie manual
  Double_t FL=theInputManager.FL;
  Double_t FG=theInputManager.FG;
  int CFD_delay=theInputManager.d; //in clock ticks
  Double_t CFD_scale_factor =theInputManager.w;
  Bool_t correctionRun =theInputManager.correction;

  CorrectionManager corMan;
  corMan.loadFile(runNum);
  Double_t SDelta_T1_Cor=corMan.get("sdt1");
  Double_t SDelta_T2_Cor=corMan.get("sdt2");
  Double_t int_corrections[4];  
  int_corrections[0]=corMan.get("int0");
  int_corrections[1]=corMan.get("int1");
  int_corrections[2]=corMan.get("int2");
  int_corrections[3]=corMan.get("int3");
  
  int degree=3;
  Double_t GOE_cor1[degree];
  Double_t GOE_cor2[degree];
  Double_t DeltaT_cor1[degree];
  Double_t DeltaT_cor2[degree];
  std::stringstream temp;
  for (int i=1;i<=degree;i++){
    temp.str("");
    temp<<"goe1_"<<i;
    GOE_cor1[i-1]=corMan.get(temp.str().c_str());
    temp.str("");
    temp<<"goe2_"<<i;
    GOE_cor2[i-1]=corMan.get(temp.str().c_str());

    temp.str("");
    temp<<"dt1_"<<i;
    DeltaT_cor1[i-1]=corMan.get(temp.str().c_str());

    temp.str("");
    temp<<"dt2_"<<i;
    DeltaT_cor1[i-1]=corMan.get(temp.str().c_str());
  }



  //prepare files and output tree
  ////////////////////////////////////////////////////////////////////////////////////
  TFile *outFile=0;
  TTree  *outT;
  FileManager * fileMan = new FileManager();
  fileMan->timingMode = theInputManager.timingMode;

  TTree * inT;
  TFile *inFile;

  inFile = new TFile(theInputManager.specificFileName.c_str());
  inT = (TTree*) inFile->Get("flt");

  
  Long64_t nentry=(Long64_t) (inT->GetEntries());
  
  cout <<"The number of entires is : "<< nentry << endl ;
  // Openning output Tree and output file
  
  stringstream s;
  s<<"CoR"<<theInputManager.specificFileName;
  outFile = fileMan->getOutputFile(s.str().c_str());


  outT = new TTree("flt","Filtered Data Tree --- Comment Description");
  cout << "Creating filtered Tree"<<endl;
  if(!outT)
    {
      cout << "\nCould not create flt Tree in " << fileMan->outputFileName.str() << endl;
      exit(-1);
    }
  ////////////////////////////////////////////////////////////////////////////////////


  LendaEvent* Event = new LendaEvent();
  LendaEvent* inEvent = new LendaEvent();

  inT->SetBranchAddress("Event",&inEvent);
  

  //Specify the output branch
  outT->Branch("Event",&Event);
  //  outT->BranchRef();
  
  if(maxentry == -1)
      maxentry=nentry;
  
  cout<<SDelta_T1_Cor<< "   "<<SDelta_T2_Cor<<endl;
  for (int jentry=0;jentry<maxentry;jentry++){
    //Get Event from tree
    inT->GetEntry(jentry);
    
    for (int i=0;i<inEvent->channels.size();i++){
      //copy over the things that are not geting changed 
      Event->pushTrace(inEvent->traces[i]);
      Event->Time_Diff = inEvent->Time_Diff;//same for all i
      Event->pushLongGate(inEvent->longGates[i]);
      Event->pushShortGate(inEvent->shortGates[i]);
      Event->pushChannel(inEvent->channels[i]);
      Event->pushEnergy(inEvent->energies[i]);
      Event->pushTime(inEvent->times[i]);
    }
    Event->pushShiftCorrections(SDelta_T1_Cor,SDelta_T2_Cor);
    Event->Finalize();
    outT->Fill();
    Event->Clear();
    
    
    //Periodic printing
    if (jentry % 10000 == 0 )
      cout<<"On event "<<jentry<<endl;
    
  }
  



outT->Write();
outFile->Close();


 

cout<<"\n\n**Finished**\n\n";

return  0;

}

