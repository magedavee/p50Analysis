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
    /// Constructor - initialize with data from another IonisationHit object
    IonisationHit(const IonisationHit &right);
    /// Destructor
    virtual ~IonisationHit();
    
    /// Assignment operator
    const IonisationHit& operator=(const IonisationHit &right);
    // /// Comparison operators
    //int operator>(const IonisationHit &right) const { return (totEDep > right.totEDep); }
    //int operator<(const IonisationHit &right) const { return (totEDep < right.totEDep); }
    //int operator>=(const IonisationHit &right) const { return (totEDep >= right.totEDep); }
    //int operator<=(const IonisationHit &right) const { return (totEDep <= right.totEDep); }
    int operator==(const IonisationHit &right) const { return copyNo==right.copyNo && volName == right.volName; }
    // /// In-place addition operator
    void operator+=(const IonisationHit &right);

    /// Called when 'new' operator is used
    inline void* operator new(size_t);
    /// called when 'delete' operator is used
    inline void operator delete(void* aHit);

    virtual void Draw() {}
    virtual void Print() {}

    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;      ///< Used to store incoming data
    virtual std::vector<G4AttValue>* CreateAttValues() const;

    void SetVolume(G4String);
    void SetEnergyDeposit(G4double E) { totEDep = E; }
    void SetEnergyEscaped(G4double Esc) { totEEsc = Esc; }
    void SetTrackID(G4int id) { trackIDs->push_back(id); }
    void SetLeft(G4bool l) { left->push_back(l); }
    G4int GetCopyNumber() const { return copyNo; }
    
    G4String GetVolumeName() const { return volName; }
    G4double GetEnergyDeposit() const { return totEDep; }
    G4double GetEnergyEscaped() const { return totEEsc; }
    std::vector<G4int>* GetTrackID() const { return trackIDs; }
    std::vector<G4int>* GetLeft() const { return left; }

private:

    G4String volName;
    G4int copyNo;
    G4double totEDep;
    G4double totEEsc;
    std::vector<G4int>* trackIDs;
    std::vector<G4int>* left;
};

/* ----------------------------------- */

typedef G4THitsCollection<IonisationHit> IonisationHitsCollection;      ///< Provides the type definition to create a hit collection

extern G4Allocator<IonisationHit> IonisationHitAllocator;               ///< Creates an allocator for this type of object

/// Allocates memory space for a IonisationHit object
inline void* IonisationHit::operator new(size_t) {
  void* aHit;
  aHit = (void*)IonisationHitAllocator.MallocSingle();
  return aHit;
}

/// De-allocates memory space previously allocated for IonisationHit objects
inline void IonisationHit::operator delete(void* aHit) {
  IonisationHitAllocator.FreeSingle((IonisationHit*) aHit);
}

#endif
