// Unrestricted Use - Property of AECL
//
// PrimaryGeneratorAction.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Source and Initial Kinematics Specifications
//      Contains definitions for functions in header file
//
// --------------------------------------------------------
//      Version 1.01 - 2011/04/29 - A. Ho
//      Edited 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#include "PrimaryGeneratorAction.hh"

#include "PrimaryGeneratorMessenger.hh" 
#include "DetectorConstruction.hh"
#include "RunAction.hh"
#include "RootIO.hh"

#include "CRYModule.hh"
#include "IBDModule.hh"
#include "FissionAntiNuModule.hh"
#include "CosmicMuonModule.hh"
#include "CosmicNeutronModule.hh"

#include <math.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>

#include <G4Event.hh>
#include <G4GeneralParticleSource.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleTypes.hh>
#include <G4ParticleDefinition.hh>
#include <G4UnitsTable.hh>
#include <Randomize.hh>
#include <G4RunManager.hh>
#include <G4UImanager.hh>
#include <G4ios.hh>


//////////////////////////////////////

void PrimaryGeneratorModule::throwPrimaries(const std::vector<primaryPtcl>& v, G4Event* anEvent) {
    if(myPGA->GetVerbosity() >= 2) G4cerr << "Throwing " << v.size() << " particles:" << G4endl;
    for(std::vector<primaryPtcl>::const_iterator it = v.begin(); it != v.end(); it++) {
        if(myPGA->GetVerbosity() >= 2) G4cerr << "\tPDG ID " << it->PDGid << "\tKE=" << G4BestUnit(it->KE,"Energy") << " at t=" << G4BestUnit(it->t,"Time") << G4endl;
        
        G4ParticleGun* gn = myPGA->GetParticleGun();
        assert(gn);
        gn->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(it->PDGid));
        gn->SetParticleEnergy(it->KE);
        gn->SetParticlePosition(it->pos);
        gn->SetParticleMomentumDirection(it->mom);
        gn->SetParticleTime(it->t);
        gn->GeneratePrimaryVertex(anEvent);
    }
}

////////////////////////////////////////

PrimaryGeneratorAction::PrimaryGeneratorAction():
XMLProvider("PrimaryGenerator"),
genModule(NULL), myCRYModule(NULL),
myIBDModule(NULL), myFisAntNuModule(NULL),
myCosmicMuonModule(NULL), myCosmicNeutronModule(NULL) {
    verbose = 0;
    
    // Particle Gun
    particle_gun = new G4ParticleGun(1);
    particle_gun->SetParticleDefinition(G4Geantino::GeantinoDefinition());

    detect = (DetectorConstruction*)(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    myMessenger = new PrimaryGeneratorMessenger(this);
}


PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete particle_gun;
    delete myMessenger;
    
    if(myCRYModule) delete myCRYModule;
    if(myIBDModule) delete myIBDModule;
    if(myFisAntNuModule) delete myFisAntNuModule;
    if(myCosmicMuonModule) delete myCosmicMuonModule;
    if(myCosmicNeutronModule) delete myCosmicNeutronModule;
}

void PrimaryGeneratorAction::loadCRYModule() {
    if(!myCRYModule) myCRYModule = new CRYModule(this);
    G4cerr << "Using CRY event generator module; remember to provide setup!" << G4endl; 
    genModule = myCRYModule;
}

void PrimaryGeneratorAction::loadIBDModule() {
    if(!myIBDModule) myIBDModule = new IBDModule(this);
    G4cerr << "Using Inverse Beta Decay event generator." << G4endl; 
    genModule = myIBDModule;
}

void PrimaryGeneratorAction::loadFisAntNuModule() {
    if(!myFisAntNuModule) myFisAntNuModule = new FissionAntiNuModule(this);
    G4cerr << "Using Fission Anti-neutrino event generator." << G4endl; 
    genModule = myFisAntNuModule;
}

void PrimaryGeneratorAction::loadCosmicMuonModule() {
    if(!myCosmicMuonModule) myCosmicMuonModule = new CosmicMuonModule(this);
    G4cerr << "Using cosmic muon event generator." << G4endl; 
    genModule = myCosmicMuonModule;
}

void PrimaryGeneratorAction::loadCosmicNeutronModule() {
    if(!myCosmicNeutronModule) myCosmicNeutronModule = new CosmicNeutronModule(this);
    G4cerr << "Using cosmic neutron event generator." << G4endl; 
    genModule = myCosmicNeutronModule;
}

void PrimaryGeneratorAction::SetVerbosity(G4int v) {
    verbose = v;
    if(verbose > 1) G4cout << "PrimaryGeneratorAction verbosity set to " << v << "." << G4endl;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    if(genModule) {
        genModule->GeneratePrimaries(anEvent);
    } else {
        assert(particle_gun);
        particle_gun->GeneratePrimaryVertex(anEvent);
    }
}

void PrimaryGeneratorAction::fillNode(TXMLEngine& E) {
    children.clear();
    if(genModule) {
        addChild(genModule);
        addAttr(E,"time",G4BestUnit(genModule->GetGeneratorTime(), "Time"));
    }
}
