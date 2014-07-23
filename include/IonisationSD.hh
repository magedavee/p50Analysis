// Unrestricted Use - Property of AECL
//
// IonisationSD.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Ionizing Radiation Scorer
//      Contains class functions/variables
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited for clarity 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#ifndef IonisationSD_H
/// Assure header file is only loaded once
#define IonisationSD_H

#include "G4VSensitiveDetector.hh"

#include "WeightAverager.hh"

#include "G4THitsMap.hh"

#include "globals.hh"

#include <map>

class G4Step;

/// Accumulate ionization track data
class IonisationHit: public WeightAverager<4> {
public:
    /// Constructor
    IonisationHit(): t(x_in[0]), x(x_in+1), VID(-1) { }
    
    double& t;          ///< time input
    double* x;          ///< position input
    double E;           ///< deposited energy input
    
    /// record contents
    void record() { fill_with_weight(E); }
    
    void SetVolume(G4int i) { VID = i; }
    
    G4int GetVolume() const { return VID; }
    G4double GetTime() const { return get_avg(0); }
    G4double GetDTime() const { return get_rms(0); }
    G4ThreeVector GetPos() const { return G4ThreeVector(get_avg(1), get_avg(2), get_avg(3)); }
    G4ThreeVector GetDPos() const { return G4ThreeVector(get_rms(1), get_rms(2), get_rms(3)); }
    G4double GetEnergyDeposit() const { return sum_w; }
    G4double GetEnergyEscaped() const { return 0; }
    
    void Display() const;
        
private:
    
    G4int VID;  ///< identifier for hit volume
};


/// Sensitive detector for tracking ionizing energy deposition
class IonisationSD : public G4VSensitiveDetector {
public:
    
    /// Constructor
    IonisationSD(G4String name): G4VSensitiveDetector(name), time_gap(20*ns), edep_threshold(100*keV), nclusters(0) { }

    /// Initializes detector at start of event
    void Initialize(G4HCofThisEvent*);
    /// Called each step to determine what qualifies as a "hit"
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*);
    /// Some processing to be done when an event is finished, as new event will erase old event data
    void EndOfEvent(G4HCofThisEvent*);

    G4double time_gap;          ///< time gap for separating event clusters
    G4double edep_threshold;    ///< threshold for registering deposited energy hit
    
private:
    
    /// Save and output a completed hit object
    void RegisterHit(IonisationHit* h);
    
    std::map< G4int, std::vector<IonisationHit*> > hit_history; ///< hits collection by detector
    uint nclusters;                                             ///< number of time-grouped event clusters
};

#endif
