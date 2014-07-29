#ifndef SCINTSD_HH
/// Assure header file is only loaded once
#define SCINTSD_HH

#include <G4VSensitiveDetector.hh>
#include "WeightAverager.hh"

#include <map>

class G4Step;

/// Accumulate ionization track data
class IonisationHit: public WeightAverager<4> {
public:
    /// Constructor
    IonisationHit(): E(0), VID(-1) { }
    
    /// record contents
    void record() { fill_with_weight(E); }
    
    void SetVolume(G4int i) { VID = i; }
    void SetTime(G4double tt) { x_in[0] = tt; }
    void SetPos(G4ThreeVector v) { for(uint i=0; i<3; i++) x_in[i+1] = v[i]; }
    void SetEnergy(G4double e) { E = e; }
        
    G4int GetVolume() const { return VID; }
    G4double GetTime() const { return get_avg(0); }
    G4double GetDTime() const { return get_rms(0); }
    G4ThreeVector GetPos() const { return G4ThreeVector(get_avg(1), get_avg(2), get_avg(3)); }
    G4ThreeVector GetDPos() const { return G4ThreeVector(get_rms(1), get_rms(2), get_rms(3)); }
    G4double GetEnergyDeposit() const { return sum_w; }
    G4double GetEnergyEscaped() const { return 0; }
    
    void Display() const;
        
protected:
    double E;   ///< deposited energy input
    G4int VID;  ///< identifier for hit volume
};


/// Sensitive detector for tracking ionizing energy deposition
class ScintSD : public G4VSensitiveDetector {
public:
    
    /// Constructor
    ScintSD(G4String name);

    /// Initializes detector at start of event
    void Initialize(G4HCofThisEvent*);
    /// Called each step to determine what qualifies as a "hit"
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* H);
    /// Some processing to be done when an event is finished, as new event will erase old event data
    void EndOfEvent(G4HCofThisEvent*);

    G4double time_gap;          ///< time gap for separating event clusters
    G4double edep_threshold;    ///< threshold for registering deposited energy hit
    
private:
    
    /// process ionizing tracks
    G4bool ProcessIoniHits(G4Step* aStep, G4TouchableHistory*);
    /// process neutron capture events
    G4bool ProcessNeutronHits(G4Step* aStep, G4TouchableHistory*);
    /// Save and output a completed ionization cluster
    void RegisterIoniHit(IonisationHit* h);
    
    G4int verbose;                                              ///< output verbosity level
    std::map<G4int, std::vector<IonisationHit*> > hit_history;  ///< ionization hits collection by detector
    std::map<G4int, G4int> secondaries_counter;                 ///< count of previously-observed secondaries in each track
    uint nclusters;                                             ///< number of time-grouped ionization event clusters
};

#endif
