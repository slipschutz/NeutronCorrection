#include <vector>
#include "TFile.h"



struct Sl_Event {
  Int_t channel;
  vector <UShort_t> trace;
  Long64_t jentry;
  Double_t time;
};



Bool_t checkChannels(vector <Sl_Event> &in){

  vector <Bool_t> ch(20,false);  //to make this work with different cable arrangements

  for (int i=0;i<in.size();i++){
      ch[in[i].channel]=true;
  }
  // if it was a good event there should be 3 trues
  //from 3 different channels
  int count=0;
  int liq_scint_count=0; //there should be only one liq scint
  // at the moment they are pluged into 8 and 9 so only one of those 
  //should be there 


  int tot =0;
  for (int i=0;i <ch.size();i++){
    if (ch[i]==true){
      count++;
      if (i==8 || i ==9 ){
	liq_scint_count++;
      } else {
	tot=tot +i;
      }
    }
  }
  
  if (count == 3 && liq_scint_count==1 && (tot==1 || tot==5 ))
    return true;
  else 
    return false;

}

void pushRollingWindow(vector <Sl_Event> &previousEvents,Double_t &sizeOfRollingWindow,
		       Double_t &time,Int_t &chanid,vector <UShort_t>& trace,Long64_t &jentry){

    //Keep the previous event info for correlating      
    if (previousEvents.size() < sizeOfRollingWindow  ) 
      {
	Sl_Event e;
	e.channel=chanid;
	e.trace=trace;
	e.jentry=jentry;
	e.time =time;
	previousEvents.push_back(e);
      }
    else if (previousEvents.size() >= sizeOfRollingWindow )
      {
	//So starting on the size+1 element 
	previousEvents.erase(previousEvents.begin(),previousEvents.begin() + 1);
	Sl_Event e;
	e.channel=chanid;
	e.trace = trace;
	e.jentry =jentry;
	e.time=time;
	previousEvents.push_back(e);	  
      }


}

Int_t getStart(Filter &theFilter,LendaEvent *theEvent,Double_t FL,Double_t FG,
	       Double_t d,Double_t w,int i){

  vector <Double_t> FF;
  vector <Double_t> CFDFilter;
  
  
  theFilter.FastFilter(theEvent->Traces[i],FF,FL,FG);

  CFDFilter= theFilter.CFD(FF,d,w);
	
  Double_t softwareCFD = theFilter.GetZeroCrossing(CFDFilter);
  return TMath::Floor(softwareCFD) -5;
}

/*
void packEvent(LendaEvent *Event,vector <Sl_Event> inEvents,
	       Filter theFilter,Double_t FL, Double_t FG,
	       int CFD_delay,Double_t CFD_scale_factor){
  
  //sort the events by channel
  
  vector <Sl_Event*> events2(16,NULL);
  vector <Sl_Event*> events;
  for (int i=0;i<inEvents.size();i++){
    events2[inEvents[i].dchan2.chanid]=&inEvents[i];
  }
  for (int i=0;i<events2.size();i++){
    if (events2[i] !=NULL )
      events.push_back(events2[i]);
  }


  vector <Double_t> thisEventsFF;
  vector <Double_t> thisEventsCFD;
  for (int i=0;i<events.size();++i){
    Double_t thisEventsIntegral=0; //intialize
    Double_t longGate=0; //intialize
    Double_t shortGate=0; //intialize
    Double_t cubicCFD=0;
    Double_t softwareCFD=0;
    Double_t traceDelay=50;
    Double_t start=0;

    thisEventsFF.clear(); //clear
    thisEventsCFD.clear();//clear
    if ((events[i]->dchan2.trace).size()!=0){ //if this event has a trace calculate filters and such
      theFilter.FastFilter(events[i]->dchan2.trace,thisEventsFF,FL,FG); //run FF algorithim
      thisEventsCFD = theFilter.CFD(thisEventsFF,CFD_delay,CFD_scale_factor); //run CFD algorithim
      
      softwareCFD=theFilter.GetZeroCrossing(thisEventsCFD)-traceDelay; //find zeroCrossig of CFD
      
      cubicCFD = theFilter.GetZeroCubic(thisEventsCFD)-traceDelay;
      
      start = TMath::Floor(softwareCFD) -5; // the start point in the trace for the gates
      thisEventsIntegral = theFilter.getEnergy(events[i]->dchan2.trace);
      // longGate = theFilter.getGate(events[i]->dchan->trace,start,25);
      // shortGate = theFilter.getGate(events[i]->dchan->trace,start,14);
            
    }
    
    Event->pushTrace(events[i]->dchan2.trace);//save the trace for later if its there
    //it is 0 if it isn't
    Event->pushFilter(thisEventsFF); //save filter if it is there
    Event->pushCFD(thisEventsCFD); //save CFD if it is there

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
  }
  
  //  Event->Finalize(); //Finalize Calculates various parameters and applies corrections
 
 
}
*/
