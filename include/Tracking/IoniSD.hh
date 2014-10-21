#ifndef IONISD_HH
#define IONISD_HH

#include "WeightAverager.hh"
#include "GeomHelper.hh"

#include <map>
#include <vector>

#include <G4VSensitiveDetector.hh>

class G4Step;
class IoniCluster;

using std::map;
using std::vector;
using std::pair;

/// Accumulate ionization track data
class IonisationHit: public WeightAverager<6> {
public:
    /// Constructor
    IonisationHit(): E(0) { }
    
    /// record contents
    void record() { fill_with_weight(E); }
    
    void SetEnergy(G4double e) { E = e; }
    void SetTime(G4double tt) { x_in[0] = tt; }
    void SetPos(G4ThreeVector v) { for(unsigned int i=0; i<3; i++) x_in[i+1] = v[i]; }
    void SetLength(G4double l) { x_in[4] = l/E; }
    void SetdEdx(G4double d) { x_in[5] = d; }
    
    G4double GetTime() const { return get_avg(0); }
    G4double GetDTime() const { return get_rms(0); }
    G4ThreeVector GetPos() const { return G4ThreeVector(get_avg(1), get_avg(2), get_avg(3)); }
    G4ThreeVector GetDPos() const { return G4ThreeVector(get_rms(1), get_rms(2), get_rms(3)); }
    G4double GetEnergyDeposit() const { return sum_w; }
    G4double GetLength() const { return get_avg(4); }
    G4double GetEdEdx() const { return sum_wx[5]; }
    G4double GetEdEdx2() const { return sum_wxx[5]; }
    
    void Display() const;
    
protected:
    double E;   ///< deposited energy input
};

/// Base class for ionization tracking Sensitive Detectors
class IoniSD {
public:
    /// Constructor
    IoniSD();
    /// Destructor
    virtual ~IoniSD() { }
    
    G4double time_gap;          ///< time gap for separating event clusters
    G4double edep_threshold;    ///< threshold for registering deposited energy hit
    G4double mat_n = 1;         ///< material electron density n/N_A/cm^3
    
protected:
    /// get PID and position information for hit
    void collectHitInfo(G4Step* aStep);
    /// process ionizing tracks. Note, returns NULL if no ionizing energy.
    IonisationHit* ProcessIoniHits(G4Step* aStep);
    /// group hits together into close-in-time clusters
    void makeClusters(vector<IonisationHit*>& hits, vector<IoniCluster>& clusts);
        
    int PID = 0;                ///< PDG particle ID
    int seg_id = 0;             ///< scintillator segment ID number
    GeomHelper W2S;             ///< world-to-scintillator coordinates helper
    G4ThreeVector worldPrePos;  ///< pre-step position in world coordinates
    G4ThreeVector worldPostPos; ///< post-step position in world coordinates
    G4ThreeVector localPrePos;  ///< pre-step position in scintillator coordinates
    G4ThreeVector localMidPos;  ///< mid-point of step in scintillator coordinates
    G4ThreeVector localPostPos; ///< post-step position in scintillator coordinates
    
    map<const G4Track*,double> parent_dEdx;     ///< dE/dx of parent track, assigned to secondaries in same volume                           
};

#endif
