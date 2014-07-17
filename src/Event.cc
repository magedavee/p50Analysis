//---------------------------------------------------
#include <iostream>
#include "TDirectory.h"
#include "TProcessID.h"
#include <vector>

#include "Event.hh"
using namespace std;

ClassImp(Event)
ClassImp(Run)
ClassImp(Track)


Event::Event()
{
  cerr<< "new Event" <<endl;
  fTracks = new TClonesArray("Track",1000);
  fNSeg = 2500;
  fNpmt = new Int_t[fNSeg];
  fSpmt = new Int_t[fNSeg];
  fSegEdep = new Double_t[fNSeg];
  fSph = 0;
  fNph=0;
  fNtrack=0;
  fEventNumber=0;
  fEventTime=0;
  fEdep = 0;
  fNcap = 0;
}

Event::Event(Int_t xx, Int_t yy)
{
  cerr<< "new event 2" <<endl;
  fTracks = new TClonesArray("Track",1000);
  fNSeg = xx*yy;
  fNpmt = new Int_t[fNSeg];
  fSpmt = new Int_t[fNSeg];
  fSegEdep = new Double_t[fNSeg];
  fSph = 0;
  fNph=0;
  fNtrack=0;
  fEventNumber=0;
  fEventTime=0;
  fEdep = 0;
  fNcap = 0;
}

Event::~Event()
{
  cerr<< "Event destructor" <<endl;
  delete fTracks;
  delete[] fNpmt;
  delete[] fSpmt;
  delete[] fSegEdep;
}

void Event::Clear(Option_t * /*option*/)
{
  fTracks->Clear("C"); //will also call Track::Clear
  // cerr<< "delete" <<endl;
  //  delete[] fNpmt;
  // delete[] fSpmt;
  //delete[] fSegEdep;
  // cerr<< "deleted" <<endl;
  fNtrack=0;
  for(int i=0;i<3;i++){
    fGenPos[i]=0;
    fGenMom[i]=0;
  }
  fEnergy=0;
  fPDGcode=0;
  for(int i=0;i<fNSeg;i++){
    fNpmt[i]=0;
    fSpmt[i]=0;
    fSegEdep[i]=0;
  }
  fSph = 0;
  fNph=0;
  fEventNumber=0;
  fEventTime=0;
  fEdep = 0;
  fNcap = 0;
}

Track::Track()
{
  for(int i=0;i<3;i++){
    tP[i] = 0;
    tPosi[i] = 0;
    tPosf[i] = 0;
  }
  tID = 0;
  tPhot = 0;
  tEi = 0;
  tEf = 0;
  tPrimary = 0;
  tParent = 0;
  tSecondary = 0;
  tStep = 0;
  tPDGcode = 0;
  tTime = 0;
  tEdep = 0;
  tSegHit = 0;
  tSegEdep.clear();
  tSegment.clear();
  /*  for(int i=0;i<2500;i++){
    tSegEdep[i] = 0;
    }  */
}

Track::Track(const Track &orig) : TObject(orig)
{
   // Copy a track object
  for(int i=0;i<3;i++){
    tP[i] = orig.tP[i];
    tPosi[i] = orig.tPosi[i];
    tPosf[i] = orig.tPosf[i];
  }
  tID = orig.tID;
  tPhot = orig.tPhot;
  tEi = orig.tEi;
  tEf = orig.tEf;
  tPrimary = orig.tPrimary;
  tParent = orig.tParent;
  tSecondary = orig.tSecondary;
  tStep = orig.tStep;
  tPDGcode = orig.tPDGcode;
  tTime = orig.tTime;
  tEdep = orig.tEdep;
  tSegHit = orig.tSegHit;
  tSegEdep= orig.tSegEdep;
  tSegment= orig.tSegment;
  /*  for(int i=0;i<2500;i++){
    tSegEdep[i] =  orig.tSegEdep[i];;
    } */ 
}

Track &Track::operator=(const Track &orig)
{
  // Copy a track
  TObject::operator=(orig);
  for(int i=0;i<3;i++){
    tP[i] = orig.tP[i];
    tPosi[i] = orig.tPosi[i];
    tPosf[i] = orig.tPosf[i];
  }
  tID = orig.tID;
  tPhot = orig.tPhot;
  tEi = orig.tEi;
  tEf = orig.tEf;
  tPrimary = orig.tPrimary;
  tParent = orig.tParent;
  tSecondary = orig.tSecondary;
  tStep = orig.tStep;
  tPDGcode = orig.tPDGcode;
  tTime = orig.tTime;
  tEdep = orig.tEdep;
  tSegHit = orig.tSegHit;
  tSegEdep= orig.tSegEdep;
  tSegment= orig.tSegment;
  /*  for(int i=0;i<2500;i++){
    tSegEdep[i] =  orig.tSegEdep[i];;
    }  */
  return *this;
}

void Track::Clear(Option_t * /*option*/)
{
   for(int i=0;i<3;i++){
    tP[i] = 0;
    tPosi[i] = 0;
    tPosf[i] = 0;
  }
  tID = 0;
  tPhot = 0;
  tEi = 0;
  tEf = 0;
  tPrimary = 0;
  tParent=0;
  tStep = 0;
  tSecondary = 0;
  tPDGcode = 0;
  tTime = 0;
  tEdep = 0;
  tSegHit = 0;
  tSegEdep.clear();
  tSegment.clear();
  tSegEdep.resize(0);
  tSegment.resize(0);
}

void Event::AddTrack(Track* tr)
{
  TClonesArray &tracks = *fTracks;
  Track *track = new(tracks[fNtrack++]) Track(*tr);
   //Save reference to last Track in the collection of Tracks
  //   return track;
  
}

Run::Run()
{
   AirGap=0;
   WrapGap=0;
   WrapThickness=0;	
   AcrylThickness=0;        
   SegBuffer=0;
   ScintLength=0;
   ScintHeight=0;
   ScintWidth=0;
   SegLength=0;
   SegWidth=0;
   SegHeight=0;	
   PMTscale=0;               
   ShieldLead=0;
   ShieldPolyB=0;
   ShieldPolyLi=0;
   Vertical=0;
   Shielded=0;
   Reflectivity=0;
   Scint=0;
}

Run::~Run()
{
 
}

void Run::Clear(Option_t * /*option*/)
{
   AirGap=0;
   WrapGap=0;
   WrapThickness=0;	
   AcrylThickness=0;        
   SegBuffer=0;
   ScintLength=0;
   ScintHeight=0;
   ScintWidth=0;
   SegLength=0;
   SegWidth=0;
   SegHeight=0;	
   PMTscale=0;               
   ShieldLead=0;
   ShieldPolyB=0;
   ShieldPolyLi=0;
   Vertical=0;
   Scint=0;
   Shielded=0;
   Reflectivity=0;
}




