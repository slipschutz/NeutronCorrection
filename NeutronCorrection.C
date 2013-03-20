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

  

  Bool_t extFlag=theInputManager.ext_flag;
  Bool_t ext_sigma_flag=theInputManager.ext_sigma_flag;

  //defualt Filter settings see pixie manual
  Double_t FL=theInputManager.FL;
  Double_t FG=theInputManager.FG;
  Double_t d=theInputManager.d; //in clock ticks
  Double_t w =theInputManager.w;

  

  Int_t long_gate = theInputManager.long_gate;
  Int_t short_gate = theInputManager.short_gate;

  Bool_t reMakePulseShape=theInputManager.reMakePulseShape;

  CorrectionManager corMan;
  corMan.loadFile(runNum);
  Double_t SDelta_T1_Cor=corMan.get("sdt1");
  Double_t SDelta_T2_Cor=corMan.get("sdt2");
  vector <Double_t> int_corrections;
  stringstream name;
  for (int i=0;i<4;i++){
    name<<"int"<<i;
    int_corrections.push_back(corMan.get(name.str().c_str()));
    name.str("");
  }
  
  int degree=3;
  Double_t GOE_cor1[degree];
  Double_t GOE_cor2[degree];
  Double_t DeltaT_cor1[degree];
  Double_t DeltaT_cor2[degree];
  vector <Double_t> Walk2_cor;
  vector <Double_t> Walk3_cor;
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

    temp.str("");
    temp<<"walk2_"<<i;
    Walk2_cor.push_back( corMan.get(temp.str().c_str()) );

    temp.str("");
    temp<<"walk3_"<<i;
    Walk3_cor.push_back( corMan.get(temp.str().c_str()) );
  }

  //prepare files and output tree
  ////////////////////////////////////////////////////////////////////////////////////
  TFile *outFile=0;
  TTree  *outT;
  FileManager * fileMan = new FileManager();
  fileMan->timingMode = theInputManager.timingMode;
  fileMan->runNum=runNum;
  TTree * inT;
  TFile *inFile;

  inFile = new TFile(theInputManager.specificFileName.c_str());
  inT = (TTree*) inFile->Get("flt");

  
  Long64_t nentry=(Long64_t) (inT->GetEntries());
  
  cout <<"The number of entires is : "<< nentry << endl ;
  // Openning output Tree and output file
  
  if (!reMakePulseShape){
    stringstream s;
    s<<"CoR"<<theInputManager.specificFileName;
    outFile = fileMan->getOutputFile(s.str().c_str());
  } else{
    outFile = fileMan->getOutputFile(long_gate,short_gate);
  }

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
  
  Event->setShiftCorrections(SDelta_T1_Cor,SDelta_T2_Cor);
  Event->setGainCorrections(int_corrections);
  Event->setWalkCorrections(Walk2_cor,2);
  Event->setWalkCorrections(Walk3_cor,3);

  Event->dumpWalkCorrections();

  Filter theFilter;
  vector <Double_t> thisEventsCFD;

  for (int jentry=0;jentry<maxentry;jentry++){ // main analysis loop
   //Get Event from tree
    inT->GetEntry(jentry);
    
    for (int i=0;i<inEvent->channels.size();i++){
      //copy over the things that are not geting changed 
      Event->pushTrace(inEvent->traces[i]);
      if (reMakePulseShape){
	Int_t start=getStart(theFilter,inEvent,FL,FG,d,w,i);
	Event->pushLongGate(theFilter.getGate(inEvent->traces[i],start,long_gate));
	Event->pushShortGate(theFilter.getGate(inEvent->traces[i],start,short_gate));
      }else {
	Event->pushLongGate(inEvent->longGates[i]);
	Event->pushShortGate(inEvent->shortGates[i]);
      }
      
      Event->pushChannel(inEvent->channels[i]);
      Event->pushEnergy(inEvent->energies[i]);
      Event->pushTime(inEvent->times[i]);
    }


    Event->Finalize();//DO important stuff
    outT->Fill();
    Event->Clear();//Oh god remember to clear
    
    
    //Periodic printing
    if (jentry % 10000 == 0 )
      cout<<"On event "<<jentry<<endl;
    
  }//End Main loop
  



outT->Write();
outFile->Close();


 

cout<<"\n\n**Finished**\n\n";

return  0;

}

