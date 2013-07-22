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

#include <iomanip>

#include <stdio.h> 
#include <ctime>

#include "functions.hh"
#define BAD_NUM -10008



using namespace std;


int main(int argc, char **argv){

  vector <string> inputs;
  for (int i=1;i<argc;++i){
    inputs.push_back(string(argv[i]));
  }
  if (inputs.size() == 0 ){ // no argumnets display helpful message
    cout<<"Usage: ./Corrector runNumber [options:value]"<<endl;
    return 0;
  }  
  
  InputManager theInputManager;
  if ( !  theInputManager.loadInputs2(inputs) ){
    return 0;
  }
  
  ////////////////////////////////////////////////////////////////////////////////////


  Int_t runNum=theInputManager.runNum;
  Long64_t maxentry=-1;
  Int_t long_gate = theInputManager.long_gate;
  Int_t short_gate = theInputManager.short_gate;





  Bool_t reMakePulseShape=theInputManager.reMakePulseShape;
  //load correcctions and settings
  CorrectionManager corMan;
  corMan.loadFile(runNum);
  Double_t SDelta_T1_Cor=corMan.get("sdt1");
  Double_t SDelta_T2_Cor=corMan.get("sdt2");
  vector <Double_t> int_corrections;
  stringstream name;
  for (int i=0;i<9;i++){
    name<<"int"<<i;
    int_corrections.push_back(corMan.get(name.str().c_str(),true));
    name.str("");
  }
  

  vector <Double_t> GOE_cor1 = corMan.GetVec("goe1");
  vector <Double_t> CGOE_cor1 = corMan.GetVec("cgoe1");
  vector <Double_t> GOE_cor2 = corMan.GetVec("goe2");
  vector <Double_t> GOE_cor3 = corMan.GetVec("goe3");


  vector <Double_t> Walk_cor0 = corMan.GetVec("walk0"); 
  vector <Double_t> Walk_cor2 = corMan.GetVec("walk2"); 
  vector <Double_t> Walk_cor3 = corMan.GetVec("walk3");
  vector <Double_t> Walk_cor1 = corMan.GetVec("walk1");


  vector <Double_t> CWalk_cor0 = corMan.GetVec("cwalk0");
  vector <Double_t> CWalk_cor1 = corMan.GetVec("cwalk1");

  
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
  

  Event->DefineMap();

  //Specify the output branch
  outT->Branch("Event",&Event);
  //  outT->BranchRef();
  
  if(maxentry == -1)
      maxentry=nentry;
  
  Event->setShiftCorrections(SDelta_T1_Cor,SDelta_T2_Cor);
  Event->setGainCorrections(int_corrections);
  Event->setWalkCorrections(Walk_cor2,2);
  Event->setWalkCorrections(Walk_cor3,3);
  Event->setWalkCorrections(Walk_cor0,0);
 


  //  Event->DefineCorrection("Dt","CorGOE",GOE_cor1,1);

//  Event->DefineCorrection("Dt_CorGOEch_1","E0",Walk_cor0,0);
 // Event->DefineCorrection("Dt_CorGOEch_1","energies[0]",Walk_cor0,0);

  Event->DefineCorrection("Dt","E0",Walk_cor0,0);
  Event->DefineCorrection("Dt","energies[0]",Walk_cor0,0);
  Event->DefineCorrection("Dt","energiesCor[0]",Walk_cor0,0);



  /*Event->DefineCorrection("Dt_CorGOEch_1_E0ch_0","E1",Walk_cor1,1);

  Event->DefineCorrection("CDt","CorGOE",CGOE_cor1,1);
  Event->DefineCorrection("CDt_CorGOEch_1","E0",CWalk_cor0,0);
  Event->DefineCorrection("CDt_CorGOEch_1_E0ch_0","E1",CWalk_cor1,1);
  */

  Event->setPositionCorrections(GOE_cor2,2);
  Event->setPositionCorrections(GOE_cor3,3);
  Event->setPositionCorrections(GOE_cor1,0);


  Event->DumpCorrectable();

  Filter theFilter;
  vector <Double_t> thisEventsCFD;

  clock_t startTime;
  clock_t otherTime;
  double timeRate=0;
  startTime = clock();
  cout<<"\n\n\n";
  
  for (int jentry=0;jentry<maxentry;jentry++){ // main analysis loop
   //Get Event from tree
    inT->GetEntry(jentry);


    *Event= *inEvent;

    Event->Finalize();//DO important stuff
    outT->Fill();
    Event->Clear();//Oh god remember to clear


    
    if (jentry % 10000 ==0 && jentry /10000 ==2){
      otherTime=clock();
      timeRate = TMath::Abs( (startTime-otherTime)/CLOCKS_PER_SEC);
      timeRate = timeRate/jentry;
    }
    //Periodic printing
    if (jentry % 10000 == 0 )
      cout<<"On event "<<setw(9)<<jentry<<" "<<setprecision(3)<<setw(4)<<((double)jentry)/maxentry*100.0<<"% seconds remaining "<<setprecision(4)<<setw(6)<<timeRate*(maxentry-jentry)<<flush<<"\r";


    
  }//End Main loop

  outT->Write();
  
  TH1F * h;
  ///Out put explicit histograms for the corrections stored in CorMap
  for ( map<string,int>::iterator ii=Event->CorMap.begin();ii != Event->CorMap.end();++ii){

    h = new TH1F(ii->first.c_str(),ii->first.c_str(),1000,-10,10);
    h->Write();
    h->SetDirectory(0);
    delete h;

  }
  

  outFile->Close();
  
  cout<<"\n\n**Finished**\n\n";
  
  return  0;

}

