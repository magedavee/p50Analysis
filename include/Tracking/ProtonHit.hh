// Unrestricted Use - Property of AECL
//
// ProtonHit.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Proton Hit Data Storage
//	Contains class functions/variables
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited for clarity 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#ifndef ProtonHit_H
/// Assure header file is only loaded once
#define ProtonHit_H

#include "G4VHit.hh"

#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include <vector>
#include <map>
#include "globals.hh"

class G4AttDef;
class G4AttValue;
using std::vector;

class ProtonHit : public G4VHit {
public:
    /// Constructor
    ProtonHit();

    /// equality operator
    int operator==(const ProtonHit &right) { return (trackID == right.trackID); }
    
    inline void* operator new(size_t) { return (void*)ProtonHitAllocator.MallocSingle(); }
    inline void operator delete(void* aHit) { ProtonHitAllocator.FreeSingle((ProtonHit*) aHit); }
    
    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual vector<G4AttValue>* CreateAttValues() const;
    
    void SetEnergy(G4double E) { energy = E; }
    void SetTrackID(G4int id) { trackID = id; }
    
    G4double GetEnergy() const { return energy; }
    G4int GetTrackID() const { return trackID; }
    
private:

    G4double energy;
    G4int trackID;
    
    static G4Allocator<ProtonHit> ProtonHitAllocator;
};

typedef G4THitsCollection<ProtonHit> ProtonHitsCollection;	// Provides the type definition to create a hit collection

#endif
