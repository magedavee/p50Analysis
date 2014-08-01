#ifndef EVENT_HH
/// Assure this header is loaded only once
#define EVENT_HH

#include <TObject.h>
#include <TClonesArray.h>
#include <TXMLEngine.h>

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

/// Neutron capture in event
class EventNCapt: public TObject {
public:
    /// constructor
    EventNCapt(): t(0.), E(0.), Ngamma(0), Egamma(0), capt_A(0), capt_Z(0), vol(0) { }
    
    Double_t t;         ///< time of capture
    Double_t E;         ///< kinetic energy at capture
    Double_t x[3];      ///< position of capture
    Int_t Ngamma;       ///< number of gammas produced
    Double_t Egamma;    ///< total energy of gammas produced
    Int_t Nprod;        ///< total number of secondary products
    Int_t capt_A;       ///< capture nucleus A
    Int_t capt_Z;       ///< capture nucleus Z
    Int_t vol;          ///< volume ID number
    
    ClassDef(EventNCapt,2);
};

class Event : public TObject {
public:
    
    /// Constructor
    Event(): N(0), nPrimaries(0), Primaries(NULL),
    nIoniClusts(0), iEvts(NULL),
    nNCapts(0), nCapts(NULL) { }
    
    /// Destructor
    ~Event();
    
    Int_t N;                    ///< event number
    Double_t t;                 ///< event time
    
    Int_t nPrimaries;           ///< number of primaries
    TClonesArray* Primaries;    ///< array of event primary particles
    
    Int_t nIoniClusts;          ///< number of ionization events
    TClonesArray* iEvts;        ///< array of event ionization events
    Double_t EIoni;             ///< total ionization deposited energy
    
    Int_t nNCapts;              ///< number of neutron captures
    TClonesArray* nCapts;       ///< array of neutron capture events
    
    /// Clear data for new event
    void Clear(Option_t *option ="");
    /// Add new primary data
    void AddPrimary(const EventPrimaryPtcl& P);
    /// Add new ionization cluster
    void AddIoniCluster(const EventIoniCluster& tr);
    /// Add new neutron capture
    void AddNCapt(const EventNCapt& n);
    
    ClassDef(Event,2);
};

/// Information about setup for simulation run
class Run : public TObject {
public:
    
    /// constructor
    Run() { Clear(); }
    
    /// reset all values to 0
    void Clear(Option_t* ="") { runNum = nEvents = simTime = 0; }
    
    Int_t runNum;       ///< run number
    Int_t nEvents;      ///< total number of events thrown
    Double_t simTime;   ///< nominal timespan simulated
    
    ClassDef(Run,2);
};

#endif
