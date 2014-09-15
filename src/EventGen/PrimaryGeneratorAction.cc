#include "PrimaryGeneratorAction.hh"

#include "PrimaryGeneratorMessenger.hh" 
#include "DetectorConstruction.hh"
#include "RunAction.hh"
#include "RootIO.hh"

#include "CRYModule.hh"
#include "IBDModule.hh"
#include "FissionAntiNuModule.hh"
#include "CosmicMuonModule.hh"
#include "Cf252Module.hh"
#include "CosmicNeutronModule.hh"
#include "GenPtclModule.hh"

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
    if(myPGA->GetVerbosity() >= 2) G4cout << "Throwing " << v.size() << " particles:" << G4endl;
    for(std::vector<primaryPtcl>::const_iterator it = v.begin(); it != v.end(); it++) {
        if(myPGA->GetVerbosity() >= 2) G4cout << "\tPDG ID " << it->PDGid << "\tKE=" << G4BestUnit(it->KE,"Energy") << " at t=" << G4BestUnit(it->t,"Time") << G4endl;
        
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
myCosmicMuonModule(NULL), myCosmicNeutronModule(NULL), myCf252Module(NULL) {
    verbose = 0;
    
    // Particle Gun
    particle_gun = new G4ParticleGun(1);
    particle_gun->SetParticleDefinition(G4Geantino::GeantinoDefinition());

    particle_source = new G4GeneralParticleSource();
    particle_source->SetNumberOfParticles(1);
    particle_source->SetParticleDefinition(G4Geantino::GeantinoDefinition());

    detect = (DetectorConstruction*)(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    myMessenger = new PrimaryGeneratorMessenger(this);
}


PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete particle_gun;
    delete particle_source;
    delete myMessenger;
    
    if(myCRYModule) delete myCRYModule;
    if(myIBDModule) delete myIBDModule;
    if(myFisAntNuModule) delete myFisAntNuModule;
    if(myCosmicMuonModule) delete myCosmicMuonModule;
    if(myCosmicNeutronModule) delete myCosmicNeutronModule;
    if(myCf252Module) delete myCf252Module;
}

void PrimaryGeneratorAction::loadCRYModule() {
    if(!myCRYModule) myCRYModule = new CRYModule(this);
    G4cout << "Using CRY event generator module; remember to provide setup!" << G4endl; 
    genModule = myCRYModule;
}

void PrimaryGeneratorAction::loadIBDModule() {
    if(!myIBDModule) myIBDModule = new IBDModule(this);
    G4cout << "Using Inverse Beta Decay event generator." << G4endl; 
    genModule = myIBDModule;
}

void PrimaryGeneratorAction::loadFisAntNuModule() {
    if(!myFisAntNuModule) myFisAntNuModule = new FissionAntiNuModule(this);
    G4cout << "Using Fission Anti-neutrino event generator." << G4endl; 
    genModule = myFisAntNuModule;
}

void PrimaryGeneratorAction::loadCosmicMuonModule() {
    if(!myCosmicMuonModule) myCosmicMuonModule = new CosmicMuonModule(this);
    G4cout << "Using cosmic muon event generator." << G4endl; 
    genModule = myCosmicMuonModule;
}

void PrimaryGeneratorAction::loadCosmicNeutronModule() {
    if(!myCosmicNeutronModule) myCosmicNeutronModule = new CosmicNeutronModule(this);
    G4cout << "Using cosmic neutron event generator." << G4endl; 
    genModule = myCosmicNeutronModule;
}

void PrimaryGeneratorAction::loadCf252Module() {
    if(!myCf252Module) myCf252Module = new Cf252Module(this);
    G4cout << "Using Cf252 event generator." << G4endl; 
    genModule = myCf252Module;
}

void PrimaryGeneratorAction::loadGPSModule() {
  if(!myGPSModule) myGPSModule = new GenPtclModule(this);
    G4cout << "Using G4GeneralParticleSource." << G4endl; 
    genModule = myGPSModule;
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
