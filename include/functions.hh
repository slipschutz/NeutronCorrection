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
  for (int i=0;i<(int)Event->times.size();++i){//can't say NumOfChannels since it hasn't been recalculated in finalize yet
    Double_t thisEventsIntegral=0; //intialize
    Double_t longGate=0; //intialize
    Double_t shortGate=0; //intialize
    Double_t cubicCFD=0;
    Double_t softwareCFD=0;
    Double_t start=0;
    thisEventsFF.clear(); //clear
    thisEventsCFD.clear();//clear
    

    if (Event->Traces[i].size()!=0){ //if this event has a trace calculate filters and such

      if (inMan.CheckOption("fl")||inMan.CheckOption("fg")||
	  inMan.CheckOption("w")||inMan.CheckOption("d")){

	theFilter.FastFilter(Event->Traces[i],thisEventsFF,FL,FG); //run FF algorithim
	thisEventsCFD = theFilter.CFD(thisEventsFF,CFD_delay,CFD_scale_factor); //run CFD algorithim
	
	softwareCFD=theFilter.GetZeroCrossing(thisEventsCFD)-traceDelay; //find zeroCrossig of CFD
	
	cubicCFD = theFilter.GetZeroCubic(thisEventsCFD)-traceDelay;
	
	//Over Write the Event with the new values 	
	Event->Filters[i]=thisEventsFF;
	Event->CFDs[i]=thisEventsCFD;
	
	Event->softTimes[i]=Event->softTimes[i]-Event->softwareCFDs[i]+softwareCFD;//take out old CFD add in new one
	Event->cubicTimes[i]=Event->cubicTimes[i]-Event->cubicCFDs[i]+cubicCFD;
	//above two lines should be before next to
	Event->softwareCFDs[i]=softwareCFD;
	Event->cubicCFDs[i]=cubicCFD;
      } else if ( inMan.CheckOption("sg")||inMan.CheckOption("lg")){
	start = TMath::Floor(Event->softwareCFDs[i])+traceDelay -4; // the start point in the trace for the gates
	thisEventsIntegral = theFilter.getEnergy(Event->Traces[i]);
	longGate = theFilter.getGate(Event->Traces[i],start,inMan.long_gate);
	shortGate = theFilter.getGate(Event->Traces[i],start,inMan.short_gate);
	//OverWrite the Event with new values 
	Event->energies[i]=thisEventsIntegral;
	Event->shortGates[i]=shortGate;
	Event->longGates[i]=longGate;

      }
    }
    
    // Event->Filters[i]=thisEventsFF; //save filter if it is there
    // Event->CFDs[i]=thisEventsCFD; //save CFD if it is there
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
