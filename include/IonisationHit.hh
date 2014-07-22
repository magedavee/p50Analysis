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

#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include <vector>
#include <map>
#include "globals.hh"

class G4AttDef;
class G4AttValue;

class IonisationHit : public G4VHit {
public:
    /// Constructor
    IonisationHit();
    
    /// equality operator
    int operator==(const IonisationHit &right) const { return copyNo == right.copyNo && volName == right.volName; }
    /// In-place addition operator
    void operator+=(const IonisationHit &right);

    /// Called when 'new' operator is used
    void* operator new(size_t) { return IonisationHitAllocator.MallocSingle(); }
    /// called when 'delete' operator is used
    void operator delete(void* aHit) { IonisationHitAllocator.FreeSingle((IonisationHit*) aHit); }

    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;      ///< Used to store incoming data
    virtual std::vector<G4AttValue>* CreateAttValues() const;

    void SetVolume(G4String);
    void SetEnergyDeposit(G4double E) { totEDep = E; }
    void SetEnergyEscaped(G4double Esc) { totEEsc = Esc; }
    void SetTrackID(G4int id) { trackIDs.push_back(id); }
    void SetLeft(G4bool l) { left.push_back(l); }
    G4int GetCopyNumber() const { return copyNo; }
    
    G4String GetVolumeName() const { return volName; }
    G4double GetEnergyDeposit() const { return totEDep; }
    G4double GetEnergyEscaped() const { return totEEsc; }

private:

    G4String volName;
    G4int copyNo;
    G4double totEDep;
    G4double totEEsc;
    std::vector<G4int> trackIDs;
    std::vector<G4int> left;
    
    static G4Allocator<IonisationHit> IonisationHitAllocator;           ///< specialized allocator for IonizationHits
};

typedef G4THitsCollection<IonisationHit> IonisationHitsCollection;      ///< Provides the type definition to create a hit collection

#endif
