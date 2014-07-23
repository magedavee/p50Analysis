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
//      Edited 2014/07 M. P. Mendenhall
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

const std::map<G4String,G4AttDef>* IonisationHit::GetAttDefs() const {
    G4bool isNew;
    std::map<G4String,G4AttDef>* store = G4AttDefStore::GetInstance("IonisationHit",isNew);
    if(isNew) {
        G4String hitType("Hit Type");
        (*store)[hitType] = G4AttDef(hitType, hitType, "Physics", "", "G4String");

        //G4String volume("Volume Name");
        //(*store)[volume] = G4AttDef(volume, volume, "Simulation", "", "G4String");

        G4String cnum("Copy Number");
        (*store)[cnum] = G4AttDef(cnum, cnum, "Simulation", "", "G4int");
        
        G4String erg("Energy Deposited");
        (*store)[erg] = G4AttDef(erg, erg, "Physics", "G4BestUnit", "G4double");
    }
    return store;
}

std::vector<G4AttValue>* IonisationHit::CreateAttValues() const {
    std::vector<G4AttValue>* values = new std::vector<G4AttValue>;

    values->push_back(G4AttValue("Hit Type","Ionisation Hit",""));
    if(VID) {
        values->push_back(G4AttValue("Copy Number",VID,""));
        //values->push_back(G4AttValue("Volume Name",VID->GetVolume()->GetName(),""));
    }
    values->push_back(G4AttValue("Energy Deposited",G4BestUnit(GetEnergyDeposit(),"Energy"),""));

    return values;
}

