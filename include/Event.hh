#ifndef __Event__
#define __Event__

#include "TObject.h"
#include "TClonesArray.h"
#include <cstdlib>
#include <vector>

using namespace std;
class TCollection;

/// Primary particle specification for ROOT output
class EventPrimaryPtcl: public TObject {
public:
    /// constructor
    EventPrimaryPtcl(): PID(0), E(0), t(0) {}
    
    Int_t PID;          ///< PDG particle ID code
    Double_t x[3];      ///< vertex position
    Double_t p[3];      ///< momentum direction
    Double_t E;         ///< kinetic energy
    Double_t t;         ///< initial time
    
    ClassDef(EventPrimaryPtcl,1);
};

/// Ionization energy deposition in event
class EventIoniCluster: public TObject {
public:
    /// constructor
    EventIoniCluster(): E(0.), t(0.), dt(0.), vol(0) {}
    
    Double_t E;         ///< deposited energy
    Double_t t;         ///< average time
    Double_t dt;        ///< RMS timing spread
    Double_t x[3];      ///< average position
    Double_t dx[3];     ///< RMS position spread
    Int_t vol;          ///< volume ID number
    
    ClassDef(EventIoniCluster,1);
};

class Event : public TObject {    
public:
    
    /// Constructor
    Event();
    /// Destructor
    ~Event();
    
    Int_t N;                    ///< event number
    Double_t t;                 ///< event time
    Double_t Edep;              ///< total scintillator deposited energy 
    Int_t nPrimaries;           ///< number of primaries
    TClonesArray* myPrimaries;  ///< array of event primary particles
    Int_t nIoniClusts;          ///< number of ionization events
    TClonesArray* myIoniClusts; ///< array of event ionization clusters
    
    /// Clear data for new event
    void Clear(Option_t *option ="");
    /// Add new ionization data
    void AddIoniCluster(const EventIoniCluster& tr) { new((*myIoniClusts)[nIoniClusts++]) EventIoniCluster(tr); }
    /// Add new primary data
    void AddPrimary(const EventPrimaryPtcl& P) { new((*myPrimaries)[nPrimaries++]) EventPrimaryPtcl(P); }
    
    ClassDef(Event,1);
};

/// Information about detector geometry for simulation run
class Run : public TObject {
public:
    
    /// constructor
    Run() { Clear(); }
    
    /// reset all values to 0
    void Clear(Option_t *option ="");
    
    Int_t NSegX;                  ///< the number of segments in x direction
    Int_t NSegY;                  ///< the number of segments in y direction

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
