// Unrestricted Use - Property of AECL
//
// IonisationHit.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Ionizing Radiation Hit Data Storage
//      Contains class functions/variables
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited for clarity 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#ifndef IonisationHit_H
/// Assure header is only loaded once
#define IonisationHit_H

#include "G4VHit.hh"
#include "WeightAverager.hh"

#include "G4UnitsTable.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TouchableHandle.hh"

#include <vector>
#include <map>
#include <set>
#include "globals.hh"

class G4AttDef;
class G4AttValue;

class IonisationHit : public G4VHit, public WeightAverager<4> {
public:
    /// Constructor
    IonisationHit(): t(x_in[0]), x(x_in+1), VID(-1) { }
    
    double& t;          ///< time input
    double* x;          ///< position input
    double E;           ///< deposited energy input
    
    /// record contents
    void record() { fill_with_weight(E); }
    
    /// Called when 'new' operator is used
    void* operator new(size_t) { return IonisationHitAllocator.MallocSingle(); }
    /// called when 'delete' operator is used
    void operator delete(void* aHit) { IonisationHitAllocator.FreeSingle((IonisationHit*) aHit); }
    
    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;

    void SetVolume(G4int i) { VID = i; }
    
    G4int GetVolume() const { return VID; }
    G4double GetTime() const { return get_avg(0); }
    G4double GetDTime() const { return get_rms(0); }
    G4ThreeVector GetPos() const { return G4ThreeVector(get_avg(1), get_avg(2), get_avg(3)); }
    G4ThreeVector GetDPos() const { return G4ThreeVector(get_rms(1), get_rms(2), get_rms(3)); }
    G4double GetEnergyDeposit() const { return sum_w; }
    G4double GetEnergyEscaped() const { return 0; }

    void Display() const {
        G4cerr << "Hit E=" << G4BestUnit(sum_w,"Energy") 
        << " at t=" << G4BestUnit(GetTime(),"Time") << "( " << G4BestUnit(GetDTime(),"Time")
        << ")\tx=[ " << G4BestUnit(GetPos(),"Length") << "] { " << G4BestUnit(GetDPos(),"Length") << "}" << G4endl; }
    
private:

    G4int VID;                                                  ///< identifier for hit volume
    
    static G4Allocator<IonisationHit> IonisationHitAllocator;   ///< specialized allocator for IonisationHits
};

#endif
