#include "PrimaryGeneratorAction.hh"

#include "DecaySourceModule.hh"

#include "PrimaryGeneratorMessenger.hh" 
#include "DetectorConstruction.hh"
#include "RunAction.hh"
#include "RootIO.hh"

#include "CRYModule.hh"
#include "IBDModule.hh"
#include "FissionAntiNuModule.hh"
#include "CosmicMuonModule.hh"
#include "Cf252Module.hh"
#include "SimpleBGModule.hh"
#include "ThermalNModule.hh"
#include "CosmicNeutronModule.hh"
#include "GenPtclModule.hh"
#include "HistogramModule.hh"

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

void PrimaryGeneratorModule::throwPrimaries(const vector<primaryPtcl>& v, G4Event* anEvent) {
    if(myPGA->GetVerbosity() >= 2) G4cout << "Throwing " << v.size() << " particles:" << G4endl;
    for(vector<primaryPtcl>::const_iterator it = v.begin(); it != v.end(); it++) {
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

int PrimaryGeneratorModule::setVertices(vector<primaryPtcl>& v) {
    return myPGA->GetPositioner()->setVertex(v);
}

bool PrimaryGeneratorModule::tryVertex(vector<primaryPtcl>& v) {
    return myPGA->GetPositioner()->tryVertex(v);
}

G4double PrimaryGeneratorModule::GetGeneratorTime() const {
    return myPGA->GetPositioner()->getAttempts() * s;
}

////////////////////////////////////////

PrimaryGeneratorAction::PrimaryGeneratorAction():
XMLProvider("PrimaryGenerator"),
detect((DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction()),
myPositioner(&myIsotPt),
myCosineThrower(NULL),
genModule(NULL), myCRYModule(NULL),
myIBDModule(NULL), myFisAntNuModule(NULL),
myCosmicMuonModule(NULL), myCosmicNeutronModule(NULL), myCf252Module(NULL) , mySimpleBGModule(NULL), myThermalNModule(NULL){
    verbose = 0;
    
    // Particle Gun
    particle_gun = new G4ParticleGun(1);
    particle_gun->SetParticleDefinition(G4Geantino::GeantinoDefinition());

    particle_source = new G4GeneralParticleSource();
    particle_source->SetNumberOfParticles(1);
    particle_source->SetParticleDefinition(G4Geantino::GeantinoDefinition());

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
    if(mySimpleBGModule) delete mySimpleBGModule;
    if(myThermalNModule) delete myThermalNModule;
    if(myDecaySourceModule) delete myDecaySourceModule;
    if(myCosineThrower) delete myCosineThrower;
}

CosineThrower* PrimaryGeneratorAction::GetCosineThrower() {
    if(!myCosineThrower) myCosineThrower = new CosineThrower(detect->theWorld);
    return myCosineThrower;
}

void PrimaryGeneratorAction::loadGunModule() {
    G4cout << "Using default particle gun event generator." << G4endl;
    genModule = NULL;
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

void PrimaryGeneratorAction::loadSimpleBGModule() {
    if(!mySimpleBGModule) mySimpleBGModule = new SimpleBGModule(this);
    G4cout << "Using SimpleBG event generator." << G4endl; 
    genModule = mySimpleBGModule;
}

void PrimaryGeneratorAction::loadThermalNModule() {
    if(!myThermalNModule) myThermalNModule = new ThermalNModule(this);
    G4cout << "Using thermal neutron event generator." << G4endl; 
    genModule = myThermalNModule;
}

void PrimaryGeneratorAction::loadGPSModule() {
  if(!myGPSModule) myGPSModule = new GenPtclModule(this);
    G4cout << "Using G4GeneralParticleSource." << G4endl; 
    genModule = myGPSModule;
}

void PrimaryGeneratorAction::loadDecaySourceModule() {
    if(!myDecaySourceModule) myDecaySourceModule = new DecaySourceModule(this);
    G4cout << "Using DecaySourceModule." << G4endl; 
    genModule = myDecaySourceModule;
}

void PrimaryGeneratorAction::loadHistogramModule() {
    if(!myHistogramModule) myHistogramModule = new HistogramModule(this);
    G4cout << "Using HistogramModule." << G4endl; 
    genModule = myHistogramModule;
}

void PrimaryGeneratorAction::SetVerbosity(G4int v) {
    verbose = v;
    if(verbose > 1) G4cout << "PrimaryGeneratorAction verbosity set to " << v << "." << G4endl;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
  if(genModule) {
    genModule->GeneratePrimaries(anEvent);
    RootIO::GetEvent().t = genModule->GetGeneratorTime();
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
    } else {
        addAttr(E,"time",G4BestUnit(1*s, "Time"));
    }
    if(myPositioner) addChild(myPositioner);
}
