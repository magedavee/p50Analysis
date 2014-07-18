#ifndef __Event__
#define __Event__

#include "TObject.h"
#include "TClonesArray.h"
#include <cstdlib>
#include <vector>

using namespace std;
class TCollection;

class Track : public TObject {

public:
  Double_t      tP[3];           //initial particle momentum
  Int_t         tID;      //track ID
  Int_t         tPhot;           //optical photons created by this track in scintillator
  Double_t      tPosi[3];        //coordinates of the first point
  Double_t      tPosf[3];        //coordinates of the last point
  Double_t      tEi;             //initial energy of the track
  Double_t      tEf;             //final energy of the track
  Short_t       tPrimary;        //is 1 if the particle is the primary, 0 otherwise
  Short_t       tSecondary;      //is 1 if the particle is a secondary, 0 otherwise
  Int_t         tParent;         // particles parent
  Int_t         tStep;         // particles parenttrack step;
  Int_t         tPDGcode;         //PDG code of the particle
  Double_t      tTime;           // creation time of particle
  Double_t      tEdep;           // energydeposited by particle in scintillator
  Int_t tSegHit;
  vector<Double_t> tSegEdep;
  vector<Int_t> tSegment;
  Track();
  Track(const Track& orig);
  virtual ~Track() {Clear();}
  Track &operator=(const Track &orig);
  void Clear(Option_t *option="");
  ClassDef(Track,1)  //A track segment
};

class Event : public TObject {
private: 

public:
  Int_t fEventNumber;
  Double_t fEventTime;
  Double_t fGenPos[3];
  Double_t fGenMom[3];
  Double_t fEnergy;
  Int_t fPDGcode;
  Int_t fNSeg;
  Int_t *fNpmt;//[fNSeg]
  Int_t *fSpmt;//[fNSeg] 
  Double_t *fSegEdep;//[fNSeg]
  Int_t fSph;
  Int_t fNph;
  Int_t fNtrack; 
  Double_t fEdep; 
  Int_t fNcap;         
  TClonesArray* fTracks;

  Event();
  Event(Int_t, Int_t);
  ~Event();
  void Clear(Option_t *option ="");
  void AddTrack(Track* tr);
  
  ClassDef(Event,1)  //Simple event class
};

/// Record of information about simulation run
class Run : public TObject {
public:
    
    /// constructor
    Run() { Clear(); }
    /// reset all values to 0
    void Clear(Option_t *option ="");
    
    Int_t NSegX;                  ///< the number of segments across (max 80)
    Int_t NSegY;                  ///< the number of segments in height (max 80)
    Double_t PositionX[2500];     ///< x position of each segment
    Double_t PositionY[2500];     ///< y position of each segment
    Double_t AirGap;              ///< air gap between segments
    Double_t WrapThickness;       ///< Thickness of Outer Tank - approximately 1/8"
    Double_t WrapGap;
    Double_t AcrylThickness;      ///< thickness of the sides of the acrylic segments
    Double_t SegBuffer;           ///< the ammount of buffer material(acrylic) between the PMT and scintillator
    Double_t ScintLength;
    Double_t ScintHeight;
    Double_t ScintWidth;
    Double_t SegLength;           ///< total length of the segment (not including PMTs) (this is defined as the z dimension)
    Double_t SegWidth;            ///< total width of the segment (defined as x)
    Double_t SegHeight;           ///< total height of the segment (defined as y) 
    Double_t PMTscale;            ///< PMT photocathode radius
    Double_t ShieldLead;          ///< thickness of lead shield layer
    Double_t ShieldPolyB;         ///< thickness of borated poly shield layer
    Double_t ShieldPolyLi;        ///< thickness of 6Li poly shield layer
    Double_t Reflectivity;
    Bool_t Vertical;              ///< whether detector is set in vertical orientation
    Bool_t Shielded;              ///< whether detector has shield layers
    Int_t Scint;                  ///< scintillator material choice

    ClassDef(Run,1)
};

#endif
