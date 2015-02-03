#include "PrimaryGeneratorAction.hh"

#include "DecaySourceModule.hh"

#include "DetectorConstruction.hh"
#include "RunAction.hh"
#include "RootIO.hh"

#include "CRYModule.hh"
#include "IBDModule.hh"
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
genModule(NULL), myCRYModule(NULL), myIBDModule(NULL),
myCosmicMuonModule(NULL), myCosmicNeutronModule(NULL),
myCf252Module(NULL) , mySimpleBGModule(NULL), myThermalNModule(NULL),
genDir("/generator/"),
modDir("/generator/module/"),
posDir("/generator/vertex/"),
verbCmd("/generator/verbose",this),
moduleGuncmd("/generator/module/gun",this),
moduleCRYcmd("/generator/module/CRY",this),
moduleIBDcmd("/generator/module/IBD",this),
moduleCosMucmd("/generator/module/CosMu",this),
moduleCosNcmd("/generator/module/CosN",this),
moduleCf252cmd("/generator/module/Cf252",this),
moduleSimpleBGcmd("/generator/module/SimpleBG",this),
moduleThermalNcmd("/generator/module/ThermalN",this),
moduleGPScmd("/generator/module/gps",this),
moduleDecaySrccmd("/generator/module/decaysrc",this),
moduleHistocmd("/generator/module/histogram",this),
ptPosCmd("/generator/vertex/isotpt", this),
isotFluxCmd("/generator/vertex/isotworld", this),
srcTargCmd("/generator/vertex/srctarg", this),
scintSrcCmd("/generator/vertex/scintvol",this),
cosFluxCmd("/generator/vertex/cosx", this),
dirFluxCmd("/generator/vertex/directional", this) {
    
    verbose = 0;
    
    // Particle Guns
    particle_gun = new G4ParticleGun(1);
    particle_gun->SetParticleDefinition(G4Geantino::GeantinoDefinition());
    particle_source = new G4GeneralParticleSource();
    particle_source->SetNumberOfParticles(1);
    particle_source->SetParticleDefinition(G4Geantino::GeantinoDefinition());


    genDir.SetGuidance("Custom simulation settings.");
    
    verbCmd.SetGuidance("Set the verbosity of this module");
    verbCmd.SetGuidance("    0 = silent, 1 = minimal, 2 = loud");
    verbCmd.SetGuidance("    Entries less than 0 will count as 0");
    verbCmd.SetGuidance("    Entries greater than 0 will also output generated values");
    verbCmd.SetParameterName("v",false);
    verbCmd.AvailableForStates(G4State_PreInit,G4State_Init,G4State_Idle);
    
    /////////////////////////////////
    // event generator module loaders
    
    moduleGuncmd.SetGuidance("Use default particle gun generator");
    moduleGuncmd.AvailableForStates(G4State_Idle);
    
    moduleCRYcmd.SetGuidance("Use CRY event generator");
    moduleCRYcmd.AvailableForStates(G4State_Idle);
    
    moduleIBDcmd.SetGuidance("Use Inverse Beta Decay event generator");
    moduleIBDcmd.AvailableForStates(G4State_Idle);
    
    moduleCosMucmd.SetGuidance("Use cosmic muon event generator");
    moduleCosMucmd.AvailableForStates(G4State_Idle);
    
    moduleCosNcmd.SetGuidance("Use cosmic neutron event generator");
    moduleCosNcmd.AvailableForStates(G4State_Idle);
    
    moduleCf252cmd.SetGuidance("Use Cf252 neutron event generator");
    moduleCf252cmd.AvailableForStates(G4State_Idle);
    
    moduleSimpleBGcmd.SetGuidance("Use SimpleBG gamma event generator");
    moduleSimpleBGcmd.AvailableForStates(G4State_Idle);
    
    moduleThermalNcmd.SetGuidance("Use thermal neutron event generator");
    moduleThermalNcmd.AvailableForStates(G4State_Idle);
    
    moduleGPScmd.SetGuidance("Use G4GeneralParticleSource generator");
    moduleGPScmd.AvailableForStates(G4State_Idle);
    
    moduleDecaySrccmd.SetGuidance("Use nuclear decay source event generator");
    moduleDecaySrccmd.AvailableForStates(G4State_Idle);
    
    moduleHistocmd.SetGuidance("Use histogram event generator");
    moduleHistocmd.AvailableForStates(G4State_Idle);
    
    ptPosCmd.SetGuidance("Generate events with isotropic momenta from specified point");
    ptPosCmd.AvailableForStates(G4State_Idle);
    
    isotFluxCmd.SetGuidance("Generate vertices for isotropic flux from world volume surface");
    isotFluxCmd.AvailableForStates(G4State_Idle);
    
    srcTargCmd.SetGuidance("Generate vertices from source to target volume");
    srcTargCmd.AvailableForStates(G4State_Idle);
    
    scintSrcCmd.SetGuidance("Generate vertices uniformly in scintillator volume");
    scintSrcCmd.AvailableForStates(G4State_Idle);
    
    cosFluxCmd.SetGuidance("Generate cos^x-weighted hemispherical flux from world volume");
    cosFluxCmd.AvailableForStates(G4State_Idle);
    
    dirFluxCmd.SetGuidance("Generate directional flux from world volume");
    dirFluxCmd.AvailableForStates(G4State_Idle);
}


PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete particle_gun;
    delete particle_source;
    
    if(myCRYModule) delete myCRYModule;
    if(myIBDModule) delete myIBDModule;
    if(myCosmicMuonModule) delete myCosmicMuonModule;
    if(myCosmicNeutronModule) delete myCosmicNeutronModule;
    if(myCf252Module) delete myCf252Module;
    if(mySimpleBGModule) delete mySimpleBGModule;
    if(myThermalNModule) delete myThermalNModule;
    if(myDecaySourceModule) delete myDecaySourceModule;
    if(myCosineThrower) delete myCosineThrower;
    if(myDirThrower) delete myDirThrower;
}

CosineThrower* PrimaryGeneratorAction::GetCosineThrower() {
    if(!myCosineThrower) myCosineThrower = new CosineThrower(detect->theWorld);
    return myCosineThrower;
}

DirectionThrower* PrimaryGeneratorAction::GetDirectionThrower() {
    if(!myDirThrower) myDirThrower = new DirectionThrower(detect->theWorld);
    return myDirThrower;
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

void PrimaryGeneratorAction::SetNewValue(G4UIcommand* command, G4String newValue) {
    
    if(command == &verbCmd) SetVerbosity(verbCmd.GetNewIntValue(newValue));
    else if(command == &moduleGuncmd) { G4cout << "Using default particle gun event generator." << G4endl; genModule = NULL; }
    else if(command == &moduleCRYcmd) loadCRYModule();
    else if(command == &moduleIBDcmd) loadIBDModule();
    else if(command == &moduleCosMucmd) loadCosmicMuonModule();
    else if(command == &moduleCosNcmd) loadCosmicNeutronModule();
    else if(command == &moduleCf252cmd) loadCf252Module();
    else if(command == &moduleSimpleBGcmd) loadSimpleBGModule();
    else if(command == &moduleThermalNcmd) loadThermalNModule();
    else if(command == &moduleGPScmd) loadGPSModule();
    else if(command == &moduleDecaySrccmd) loadDecaySourceModule();
    else if(command == &moduleHistocmd) loadHistogramModule();
    
    else if(command == &ptPosCmd) {
        myPositioner = &myIsotPt;
        myIsotPt.setPos(ptPosCmd.GetNew3VectorValue(newValue));
    } else if(command == &isotFluxCmd || command == &cosFluxCmd) {
        myPositioner = GetCosineThrower();
        GetCosineThrower()->setSourceTarget(NULL,NULL);
        GetCosineThrower()->setExponent(command == &cosFluxCmd? cosFluxCmd.GetNewDoubleValue(newValue) : 0);
    } else if(command == &srcTargCmd) {
        myPositioner = GetCosineThrower();
        GetCosineThrower()->fromVolume = true;
        GetCosineThrower()->setSourceTarget(GetDetector()->ptclSrc, GetDetector()->ptclTrg);
        GetCosineThrower()->setExponent(0);
        if(myIBDModule && genModule == myIBDModule) GetCosineThrower()->reScatter = true;
    } else if(command == &scintSrcCmd) {
        ScintSegVol* V = GetDetector()->getScint();
        G4VPhysicalVolume* scph = V? V->scint_phys : NULL;
        if(!scph) { G4cout << "Scintillator not defined in this geometry!" << G4endl; return; }
        myPositioner = GetCosineThrower();
        GetCosineThrower()->fromVolume = true;
        GetCosineThrower()->setSourceTarget(scph, NULL);
        GetCosineThrower()->setExponent(0);
    } else if(command == &dirFluxCmd) {
        myPositioner = GetDirectionThrower();
        GetDirectionThrower()->setDirection(dirFluxCmd.GetNew3VectorValue(newValue));
    }
    
    else G4cout << "Command not found." << G4endl;
}
