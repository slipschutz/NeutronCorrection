
#include "FileManager.h"

#include <iostream>
#include <sstream>

#include "TFile.h"
#include <stdlib.h>

using namespace std;
FileManager::FileManager()
{
  //Constructor for file manager

  outputFileName.str("Defualt");
  fileName.str("Defualt");
  timingMode="softwareCFD";//Default to softwareCFD

  runNum=-1;

}
void FileManager::setFileWithRun(){
  
  if (runNum ==-1){
    cout<<"***Warning no runNumber set***"<<endl;
    cout<<"FileManager->runNum=ruNum or load file"<<endl;
  } else {
    if (runNum <10){
      fileName<<"run-000"<< runNum;
    } else if (runNum < 100){
      fileName<<"run-00"<< runNum;
    } else if (runNum <1000) {
      fileName<<"run-0"<< runNum;
    } else {
      cout<<"***Warning Update run number parsing***"<<endl;
    }
  }
}


TString FileManager::loadFile(Int_t runNum,Int_t fileNum) {

  //set run num
  this->runNum=runNum;

  fileName.str("");
  outputFileName.str("");
  setFileWithRun();

  
  outputFileName << fileName.str()<<"-"<<timingMode;

  if (fileNum < 10){
    fileName<<"-0"<<fileNum;
  }else if (fileNum >=10 ){
    fileName<<"-"<<fileNum;
  }
    

  fileName << ".root";
  
  cout<<"Loading file "<<fileName.str()<<"..."<<endl;
  
  
  return fileName.str();


}

TFile * FileManager::getOutputFile(){


  outputFileName << "-output.root";

  TFile * temp = new TFile(outputFileName.str().c_str(),"recreate");
   
  if(!temp)
    {
      cout << "\nCould not open " << outputFileName.str() <<endl;
      exit(-1);
    } else
      cout << "Opened output file " <<outputFileName.str()<< endl;

  return temp;
}

TFile * FileManager::getOutputFile(Double_t FL, Double_t FG, Double_t d, Double_t w){

  std::stringstream st;

  st<<"FL"<<FL<<"FG"<<FG<<"d"<<d<<"w"<<w;

  st << outputFileName.str()<<".root";
  
  outputFileName.str(st.str());
  
  TFile * temp = new TFile(outputFileName.str().c_str(),"recreate");
   
  if(!temp)
    {
      cout << "\nCould not open " << outputFileName.str() <<endl;
      exit(-1);
    } else
    cout << "Opened output file " <<outputFileName.str()<< endl;

  return temp;
}

TFile * FileManager::getOutputFile(Double_t sigma){
  std::stringstream st;

  st<<"sigma_"<<sigma<<"_";

  st << outputFileName.str()<<".root";

  outputFileName.str(st.str());

  TFile * temp = new TFile(outputFileName.str().c_str(),"recreate");

  if(!temp)
    {
      cout << "\nCould not open " << outputFileName.str() <<endl;
      exit(-1);
    } else
    cout << "Opened output file " <<outputFileName.str()<< endl;

  return temp;


}
TFile *FileManager::getOutputFile(string s){
  TFile* temp =new TFile(s.c_str(),"recreate");
  outputFileName.str(s);

  if(!temp)
    {
      cout << "\nCould not open " << outputFileName.str() <<endl;
      exit(-1);
    } else
    cout << "Opened output file " <<outputFileName.str()<< endl;

  return temp;

}

TFile * FileManager::getOutputFile(Int_t LG,Int_t SG){

 
  setFileWithRun();//sets the fileName to run-0num

  outputFileName.str("");
  outputFileName<<fileName.str()<<"-LG"<<LG<<"-SG"<<SG<<".root";
  
  TFile *temp =new TFile(outputFileName.str().c_str(),"recreate");
  
  return temp;

}
