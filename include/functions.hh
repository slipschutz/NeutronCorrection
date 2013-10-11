#include <vector>
#include "TFile.h"



struct Sl_Event {
  Int_t channel;
  vector <UShort_t> trace; 
  Long64_t jentry;
  Double_t time;
};


Int_t getStart(Filter &theFilter,LendaEvent *theEvent,Double_t FL,Double_t FG,
	       Double_t d,Double_t w,int i){

  vector <Double_t> FF;
  vector <Double_t> CFDFilter;
  
  
  theFilter.FastFilter(theEvent->Traces[i],FF,FL,FG);

  CFDFilter= theFilter.CFD(FF,d,w);
	
  Double_t softwareCFD = theFilter.GetZeroCrossing(CFDFilter);
  return TMath::Floor(softwareCFD) -5;
}

void RePackEvent(LendaEvent *Event,Filter &theFilter,InputManager& inMan){
  Double_t FL =inMan.FL;
  Double_t FG=inMan.FG;
  int CFD_delay=inMan.d;
  Double_t CFD_scale_factor=inMan.w;
  Double_t traceDelay=inMan.traceDelay;

  //  bool lean =inMan.lean;


  vector <Double_t> thisEventsFF;
  vector <Double_t> thisEventsCFD;
  for (int i=0;i<(int)Event->NumOfChannelsInEvent;++i){
    Double_t thisEventsIntegral=0; //intialize
    Double_t longGate=0; //intialize
    Double_t shortGate=0; //intialize
    Double_t cubicCFD=0;
    Double_t softwareCFD=0;

    Double_t start=0;

    thisEventsFF.clear(); //clear
    thisEventsCFD.clear();//clear
    if (Event->Traces[i].size()!=0 &&inMan.fast==false){ //if this event has a trace calculate filters and such
      theFilter.FastFilter(Event->Traces[i],thisEventsFF,FL,FG); //run FF algorithim
      thisEventsCFD = theFilter.CFD(thisEventsFF,CFD_delay,CFD_scale_factor); //run CFD algorithim
      
      softwareCFD=theFilter.GetZeroCrossing(thisEventsCFD)-traceDelay; //find zeroCrossig of CFD
      
      cubicCFD = theFilter.GetZeroCubic(thisEventsCFD)-traceDelay;
      
      start = TMath::Floor(softwareCFD)+traceDelay -5; // the start point in the trace for the gates
      thisEventsIntegral = theFilter.getEnergy(Event->Traces[i]);
      longGate = theFilter.getGate(Event->Traces[i],start,25);
      shortGate = theFilter.getGate(Event->Traces[i],start,14);
            
    }
    
    Event->Filters[i]=thisEventsFF; //save filter if it is there
    Event->CFDs[i]=thisEventsCFD; //save CFD if it is there
    /*
    //Push other thing into the event
    Event->pushLongGate(longGate); //longer integration window
    Event->pushShortGate(shortGate);//shorter integration window
    Event->pushChannel(events[i]->dchan2.chanid);//the channel for this pulse
    Event->pushEnergy(thisEventsIntegral);;//push trace energy if there
    Event->pushInternEnergy(events[i]->dchan2.energy);//push internal energy
    Event->pushTime(events[i]->dchan2.time);
    Event->pushSoftTime(events[i]->dchan2.timelow +events[i]->dchan2.timehigh*4294967296.0+softwareCFD);
    Event->pushSoftwareCFD(softwareCFD);
    Event->pushCubicTime(events[i]->dchan2.timelow +events[i]->dchan2.timehigh*4294967296.0+cubicCFD);
    Event->pushInternalCFD((events[i]->dchan2.timecfd)/65536.0);
    Event->pushEntryNum(events[i]->jentry);
    */  
  }

}

string GetFormatedNumber(double num,int length,int precision){
  
  double decimalPart = num-floor(num);
  /*
  decimalPart= ( round ( decimalPart*(pow(10,precision))) )/(pow(10,precision));
  cout<<"decimalPart is "<<decimalPart<<endl;
  
  stringstream decimalPartStream <<decimalPart;
  int decimalPartLength = decimalPartStream.str().length();
  
  stringstream numberPartStream << floor(num);
  int numberPartLength = numberPartStream.str().length();

  
  */
  

  
  string s= "hello";
  return s; 
  


}
