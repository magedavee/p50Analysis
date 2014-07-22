// Unrestricted Use - Property of AECL
//
// IonisationHit.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Ionizing Radiation Hit Data Storage
//      Contains definitions for functions in header file
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited for clarity 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#include "IonisationHit.hh"

#include "G4RunManager.hh"
#include "G4VVisManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4Colour.hh"
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

G4Allocator<IonisationHit> IonisationHit::IonisationHitAllocator;

IonisationHit::IonisationHit() {
    volName = "";
    copyNo = -1;
    totEDep = 0.*MeV;
    totEEsc = 0.*MeV;
}

void IonisationHit::operator+=(const IonisationHit &right) {
    totEDep += right.totEDep;
    totEEsc += right.totEEsc;
    
    G4bool IsNew = true;
    for(std::vector<G4int>::iterator itr = trackIDs.begin(); itr != trackIDs.end(); itr++) {
        if((*itr) == right.trackIDs[0]) {
            IsNew = false;
            break;
        }
    }
    
    for(std::vector<G4int>::iterator itr = left.begin(); itr != left.end(); itr++) {
        if((*itr) == right.trackIDs[0]) {
            left.erase(itr);
            break;
        }
    }

    if(IsNew) trackIDs.push_back(right.trackIDs[0]);
    if(!right.left.empty()) left.push_back(right.left[0]);
}

// The following functions specify and assemble the memory allocations required/variables stored for an object of this class

const std::map<G4String,G4AttDef>* IonisationHit::GetAttDefs() const {
    G4bool isNew;
    std::map<G4String,G4AttDef>* store = G4AttDefStore::GetInstance("IonisationHit",isNew);
    if(isNew) {
        G4String hitType("Hit Type");
        (*store)[hitType] = G4AttDef(hitType,"Hit Type","Physics","","G4String");

        G4String volume("Volume Name");
        (*store)[volume] = G4AttDef(volName,"Volume Name","Simulation","","G4String");

        G4String erg("Energy Deposited");
        (*store)[erg] = G4AttDef(erg,"Energy Deposited","Physics","G4BestUnit","G4double");

        G4String escape("Energy Escaped");
        (*store)[escape] = G4AttDef(escape,"Energy Escaped","Physics","G4BestUnit","G4double");

        G4String id("Track ID");
        (*store)[id] = G4AttDef(id,"Track ID","Simulation","","std::vector");

        G4String track("Tracker");
        (*store)[track] = G4AttDef(track,"Tracker","Simulation","","std::vector");
    }
    return store;
}

std::vector<G4AttValue>* IonisationHit::CreateAttValues() const {
    std::vector<G4AttValue>* values = new std::vector<G4AttValue>;

    values->push_back(G4AttValue("Hit Type","Ionisation Hit",""));
    values->push_back(G4AttValue("Copy Number",copyNo,""));
    values->push_back(G4AttValue("Volume Name",volName,""));
    values->push_back(G4AttValue("Energy Deposited",G4BestUnit(totEDep,"Energy"),""));
    values->push_back(G4AttValue("Energy Escaped",G4BestUnit(totEEsc,"Energy"),""));
    values->push_back(G4AttValue("Track ID","Vector",""));
    values->push_back(G4AttValue("Tracker","Vector",""));

    return values;
}

void IonisationHit::SetVolume(G4String volume) {
    volName = volume;
    G4VPhysicalVolume* detVolume = G4PhysicalVolumeStore::GetInstance()->GetVolume(volName);
    if(detVolume) {
        G4int numCopy = detVolume->GetCopyNo();
        if(numCopy >= 0) copyNo = numCopy;
    }
}
