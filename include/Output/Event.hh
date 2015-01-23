/// \file Event.hh \brief ROOT TTree output classes.
#ifndef EVENT_HH
/// Assure this header is loaded only once
#define EVENT_HH

#include <TObject.h>
#include <vector>
#include <map>
using std::vector;
using std::map;

class TClonesArray;

/// Primary particle specification for ROOT output
class ParticleVertex: public TObject {
public:
    /// constructor
    ParticleVertex(): PID(0), E(0), t(0) {}
    
    Int_t PID;          ///< PDG particle ID code
    Double_t x[3];      ///< vertex position
    Double_t p[3];      ///< momentum direction
    Double_t E;         ///< kinetic energy
    Double_t t;         ///< initial time
    
    ClassDef(ParticleVertex,2);
};

/// Event containing a list of particles
class ParticleEvent: public TObject {
public:
    /// Constructor
  ParticleEvent(): nParticles(0), particles(NULL) { Clear(""); }
    /// Destructor
    ~ParticleEvent();
    
    Int_t nParticles;           ///< number of particles
    TClonesArray* particles;    ///< the particles
    
    /// Clear data for new event
    void Clear(Option_t *option ="");
    /// Add new particle data
    void AddParticle(const ParticleVertex& P);
    
    ClassDef(ParticleEvent,1);
};

/// Ionization energy deposition in event
class IoniCluster: public TObject {
public:
    /// constructor
    IoniCluster(): E(0.), t(0.), dt(0.), EdEdx(0), EdEdx2(0), vol(0), PID(0) {}
    
    Double_t E;         ///< deposited energy
    Double_t t;         ///< average time
    Double_t dt;        ///< RMS timing spread
    Double_t x[3];      ///< average position
    Double_t dx[3];     ///< RMS position spread
    Double_t EdEdx;     ///< approximated energy-weighted \f$dE/dx\f$ \f$\int dE/dx dE\f$ for quenching calculation
    Double_t EdEdx2;    ///< approximated energy-weighted \f$(dE/dx)^2\f$ \f$\int (dE/dx)^2 dE\f$ for quenching calculation
    Double_t l;         ///< track length
    Int_t vol;          ///< volume ID number
    Int_t PID;          ///< ionizing particle type
    
    /// energy-weighted sum
    void operator+=(const IoniCluster& r);
    /// total position spread in all axes
    Double_t dxtot() const;
    /// quenched energy approximation
    Double_t Equench() const;
    
    ClassDef(IoniCluster,4);
};

/// Event containing a list of ionization clusters
class IoniClusterEvent: public TObject {
public:
    
    /// Constructor
    IoniClusterEvent(): nIoniClusts(0), clusts(NULL) { Clear(""); }
    
    /// Destructor
    ~IoniClusterEvent();
    
    /// Clear data for new event
    void Clear(Option_t *option ="");
    /// Add new ionization cluster
    void AddIoniCluster(const IoniCluster& tr);
    
    Int_t nIoniClusts;          ///< number of ionization events
    TClonesArray* clusts;       ///< array of event ionization events
    Double_t EIoni;             ///< total ionization deposited energy
    
    ClassDef(IoniClusterEvent,1);
};

/// Neutron capture in event
class NCapt: public TObject {
public:
    /// constructor
    NCapt(): t(0.), E(0.), Ngamma(0), Egamma(0), Nprod(0), capt_A(0), capt_Z(0), vol(0) { }
    
    Double_t t;         ///< time of capture
    Double_t E;         ///< kinetic energy at capture
    Double_t x[3];      ///< position of capture
    Int_t Ngamma;       ///< number of gammas produced
    Double_t Egamma;    ///< total energy of gammas produced
    Int_t Nprod;        ///< total number of secondary products
    Int_t capt_A;       ///< capture nucleus A
    Int_t capt_Z;       ///< capture nucleus Z
    Int_t vol;          ///< volume ID number
    
    /// Sort by time. Returns 0 when equal, -1 when this is smaller and +1 when bigger.
    virtual Int_t Compare(const TObject* obj) const;
    /// Identify that these are sortable objects
    virtual Bool_t IsSortable() const { return kTRUE; }
    
    ClassDef(NCapt,1);
};

/// Event containing a list of neutron captures
class NCaptEvent: public TObject {
public:
    /// Constructor
    NCaptEvent(): nNCapts(0), nCapts(NULL) { Clear(""); }
    /// Destructor
    ~NCaptEvent();
    
    Int_t nNCapts;              ///< number of neutron captures
    TClonesArray* nCapts;       ///< array of neutron capture events
    
    /// Clear data for new event
    void Clear(Option_t *option ="");
    /// Add new neutron capture
    void AddNCapt(const NCapt& n);
    
    ClassDef(NCaptEvent,1);
};

/// Basic event header information
class Event: public TObject {
public:
    
    /// Constructor
    Event(): N(0), t(0) { }
    
    Int_t N;            ///< event number
    Double_t t;         ///< event time
    Double_t ct;        ///< computer time to calculate event
    Int_t flg;          ///< event flags
    
    /// flags for event status
    enum evtFlags {
        EVT_TRAPPED = 1 << 0    ///< event is "trapped" (exceeds computation time limit)
    };
    
    /// Clear data for new event
    void Clear(Option_t* ="") { N = t = ct = flg = 0; }
    
    ClassDef(Event,5);
};

/// merge ionization events into single history; return total in each volume
map<Int_t, double> mergeIoniHits(TClonesArray* clusts, vector<IoniCluster>& hitHist, double dt_max);

/// merge ionization events into single history, regardless of volume
vector<IoniCluster> mergeIoniHits(const vector<IoniCluster>& hts, double dt_max);

/// identified hit classifications
enum HitTypeID {
    IONI_HIT,       ///< electron/gamma ionization
    NCAPT_HIT,      ///< neutron capture hit
    RECOIL_HIT,     ///< nucleus recoil
    VETO_HIT,       ///< veto detector hit
    DEAD_HIT,       ///< dead volume or non-detected hit
    CRAZY_HIT       ///< unclassifiable event
};

/// classify observed "hit type" of ionization cluster
HitTypeID classifyHit(const IoniCluster& h);

#endif
