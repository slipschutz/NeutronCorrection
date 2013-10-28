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
#include "TF1.h"
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
  Long64_t maxentry=theInputManager.maxEntry;
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
  /*  for (int i=0;i<9;i++){
    name<<"int"<<i;
    int_corrections.push_back(corMan.get(name.str().c_str(),true));
    name.str("");
    }*/
  
  
  

  vector <Double_t> GOE_cor1 = corMan.GetVec("goe1");

  vector <Double_t> CGOE_cor1 = corMan.GetVec("cgoe1");
  vector <Double_t> CGOE_cor2 = corMan.GetVec("cgoe2");

  vector <Double_t> GOE_cor2 = corMan.GetVec("goe2");
  vector <Double_t> GOE_cor3 = corMan.GetVec("goe3");

 
  vector <Double_t> Walk_cor0 = corMan.GetVec("walk0"); 
  vector <Double_t> Walk_cor2 = corMan.GetVec("walk2"); 
  vector <Double_t> Walk_cor3 = corMan.GetVec("walk3");
  vector <Double_t> Walk_cor1 = corMan.GetVec("walk1");


  vector <Double_t> CWalk_cor0 = corMan.GetVec("cwalk0");
  vector <Double_t> CWalk_cor1 = corMan.GetVec("cwalk1");
  vector <Double_t> CWalk_cor2 = corMan.GetVec("cwalk2");
  vector <Double_t> CWalk_cor3 = corMan.GetVec("cwalk3");

  
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

  //  Event->setShiftCorrections(SDelta_T1_Cor,SDelta_T2_Cor);
  // Event->setGainCorrections(3.03949e-01,1.01169e+01,0);
  // Event->setGainCorrections(3.03949e-01,1.01169e+01,1);
  // Event->setGainCorrections(3.03949e-01,1.01169e+01,2);
  // Event->setGainCorrections(3.03949e-01,1.01169e+01,3);



 

  vector <Double_t> temp33;
  temp33.push_back(-0.0580705);
  temp33.push_back(0.269542);

  //  CGOE_cor1
  /*  Event->DefineCorrection("TOF","CDt",temp33,0);
  
  Event->DefineCorrection("TOF_CDtch0","energiesCor[0]",CWalk_cor0,1);

  Event->DefineCorrection("TOF_CDtch0_energiesCor[0]ch1","energiesCor[1]",CWalk_cor1,1);
  Event->DefineCorrection("TOF_CDtch0_energiesCor[0]ch1_energiesCor[1]ch1","energiesCor[2]",CWalk_cor2,1);
  */

/*Event->DefineCorrection("TOF_energies[0]ch1_energies[1]ch1","energies[2]",CWalk_cor2,1);
  Event->DefineCorrection("TOF_energies[0]ch1_energies[1]ch1_energies[2]ch1","energies[3]",CWalk_cor2,1);
  */

 // Event->DefineCorrection("CDt_CorGOEch1","CorGOE", CGOE_cor2,1);
  //  Event->DefineCorrection("CDt_CorGOEch1_CorGOEch1","E0",CWalk_cor0,1);
  

//  Event->DefineCorrection("Dt_CorGOEch_1","E0",Walk_cor0,0);
 // Event->DefineCorrection("Dt_CorGOEch_1","energies[0]",Walk_cor0,0);

  //  Event->DefineCorrection("Dt","E0",Walk_cor0,0);
  //  Event->DefineCorrection("Dt","energies[0]",Walk_cor0,0);
  //  Event->DefineCorrection("Dt","energiesCor[0]",Walk_cor0,0);



  /*Event->DefineCorrection("Dt_CorGOEch_1_E0ch_0","E1",Walk_cor1,1);

  Event->DefineCorrection("CDt","CorGOE",CGOE_cor1,1);
  Event->DefineCorrection("CDt_CorGOEch_1","E0",CWalk_cor0,0);
  Event->DefineCorrection("CDt_CorGOEch_1_E0ch_0","E1",CWalk_cor1,1);
  */
  /*
  Event->setPositionCorrections(GOE_cor2,2);
  Event->setPositionCorrections(GOE_cor3,3);
  Event->setPositionCorrections(GOE_cor1,0);
  */

  Event->DumpCorrectable();

  Filter theFilter;
  vector <Double_t> thisEventsCFD;

  clock_t startTime;
  clock_t otherTime;
  clock_t timePrevious;
  Double_t timeRate=0;
  startTime = clock();

  Double_t countThresh =floor(0.005 * maxentry);//the spot at which the time estimate is made

  cout<<"\n\n\n";
  inT->GetEntry(0);
  
  if(theInputManager.CheckOption("gammapeak")){
    Event->SetGammaPeakTime(theInputManager.GammaPeak);
  }else {
    Event->SetGammaPeakTime(inEvent->GammaPeakTime);
  }
  for (int jentry=0;jentry<maxentry;jentry++){ // main analysis loop

  //Get Event from tree
    inT->GetEntry(jentry);
    if (inEvent->NumOfChannelsInEvent==3 || (inEvent->NumOfChannelsInEvent==2 && inEvent->channels[0]==8 )){
      //copy the event 
      *Event= *inEvent;
      // cout<<"Event "<<Event->Traces.size()<<endl;
      // cout<<"inEvent "<<inEvent->Traces.size()<<endl;
      //Redo the PulseShape
      // for (int i=0;i<Event->Traces.size();i++){
      // 	if ( Event->channels[i]==8 ||  Event->channels[i]==9){
      // 	  int start = TMath::Floor(Event->softwareCFDs[i])+theInputManager.traceDelay -4;
      // 	  Double_t longGate = theFilter.getGate(Event->Traces[i],start,theInputManager.long_gate);
      // 	  Double_t shortGate = theFilter.getGate(Event->Traces[i],start,theInputManager.short_gate);
      // 	  Event->longGates[i]=longGate;//over write old long gate
      // 	  Event->shortGates[i]=shortGate;
      // 	}
      // }
      RePackEvent(Event,theFilter,theInputManager);

      Event->Finalize();//DO important stuff
      outT->Fill();
      Event->Clear();//Oh god remember to clear
    }
    
    
    if (jentry == countThresh ){
      otherTime=clock();
      timeRate = (Double_t)TMath::Abs( (startTime-otherTime)/CLOCKS_PER_SEC);
      timeRate = (Double_t)timeRate/((Double_t)jentry);
    }
    //Periodic printing
    if (jentry % 10000 == 0 ){
      //      cout<<"On event "<<setw(9)<<jentry<<" "<<setprecision(3)<<setw(4)<<((double)jentry)/maxentry*100.0<<"% seconds remaining "<<setprecision(4)<<setw(6)<<timeRate*(maxentry-jentry)<<flush<<"\r";
      cout<<flush<<"\r"<<"                                                                                          "<<"\r";
      cout<<setprecision(4)<<"On Event "<<jentry<<" "<<((double)jentry)/(maxentry)*100<<"% minutes remaining "<<(1.0/60)*timeRate*(maxentry-jentry)<<" hours remaining "<<(1.0/3600)*timeRate*(maxentry-jentry);

    }

 
  }//End Main loop

  outT->Write();
  /*  
  TH1F * h;
  ///Out put explicit histograms for the corrections stored in CorMap
  for ( map<string,int>::iterator ii=Event->CorMap.begin();ii != Event->CorMap.end();++ii){

    h = new TH1F(ii->first.c_str(),ii->first.c_str(),1000,-10,10);
    h->Write();
    h->SetDirectory(0);
    delete h;

  }
  */

  outFile->Close();
  
  cout<<"\n\n**Finished**\n\n";
  
  return  0;

}

