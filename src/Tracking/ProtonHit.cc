// Unrestricted Use - Property of AECL
//
// ProtonHit.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Proton Hit Data Storage
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "ProtonHit.hh"

#include <G4SystemOfUnits.hh>
#include "G4RunManager.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UnitsTable.hh"
#include "G4VisAttributes.hh"
#include "G4ThreeVector.hh"
#include "G4Point3D.hh"
#include "G4Circle.hh"

#include <iomanip>
#include "G4ios.hh"

#include "globals.hh"
#include <vector>

G4Allocator<ProtonHit> ProtonHit::ProtonHitAllocator;

ProtonHit::ProtonHit() {
    energy = 0.*MeV;
    trackID = -1;
}

const std::map<G4String,G4AttDef>* ProtonHit::GetAttDefs() const {
    G4bool isNew;
    std::map<G4String,G4AttDef>* store = G4AttDefStore::GetInstance("ProtonHit",isNew);
    if(isNew) {
        G4String hitType("Hit Type");
        (*store)[hitType] = G4AttDef(hitType,"Hit Type","Physics","","G4String");
        
        G4String erg("Proton Energy");
        (*store)[erg] = G4AttDef(erg,"Proton Energy","Physics","G4BestUnit","G4double");
        
        G4String id("Track ID");
        (*store)[id] = G4AttDef(id,"Track ID","Simulation","","G4int");
    }
    return store;
}

std::vector<G4AttValue>* ProtonHit::CreateAttValues() const {
    std::vector<G4AttValue>* values = new std::vector<G4AttValue>;
    
    values->push_back(G4AttValue("Hit Type","Proton Hit",""));
    values->push_back(G4AttValue("Proton Energy",G4BestUnit(energy,"Energy"),""));
    values->push_back(G4AttValue("Track ID",trackID,""));
    
    return values;
}
