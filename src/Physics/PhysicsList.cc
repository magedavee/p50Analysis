#include "PhysicsList.hh"

#ifndef EMPHYSICS

PhysicsList::PhysicsList(): XMLProvider("Physics"),
myMessenger(new PhysicsListMessenger(this)), myOptPhys(NULL) {
}

PhysicsList::~PhysicsList() {
    delete myMessenger;
    // Do not delete myOptPhys --- automatically handled
}

void PhysicsList::fillNode(TXMLEngine& E) {
    addAttr(E, "optical", myOptPhys?"on":"off");
}

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* p): 
phys(p), physDir(new G4UIdirectory("/phys/")) {    
    opticalCmd = new G4UIcmdWithoutParameter("/phys/enableOptical",this);
    opticalCmd->SetGuidance("Enables optical photon physics.");
    opticalCmd->AvailableForStates(G4State_PreInit,G4State_Init);
}

PhysicsListMessenger::~PhysicsListMessenger() {
    delete physDir;
    delete opticalCmd;
}

void PhysicsListMessenger::SetNewValue(G4UIcommand* command, G4String) {
    if(command == opticalCmd) {
        if(!phys->myOptPhys) {
            phys->myOptPhys = new G4OpticalPhysics(0);
            phys->RegisterPhysics(phys->myOptPhys);
        }
    }
}

#else

#include "StepMax.hh"
#include <G4SystemOfUnits.hh>
#include <G4ProductionCutsTable.hh>
#include <G4ProcessManager.hh>

void PhysicsList::SetCuts() {
    G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(250*eV, 1*GeV);
    double rangecut = 1*um;
    SetCutValue(rangecut, "gamma");
    SetCutValue(rangecut, "e-");
    SetCutValue(rangecut, "e+");
}

void PhysicsList::AddStepMax() {
    StepMax* stepMaxProcess = new StepMax();
    
    printf("Setting step-size limits...\n");
    
    theParticleIterator->reset();
    while ((*theParticleIterator)()){
        G4ParticleDefinition* particle = theParticleIterator->value();
        G4ProcessManager* pmanager = particle->GetProcessManager();
        if (stepMaxProcess->IsApplicable(*particle)) {
            printf("\tSetting step limit for %s\n", particle->GetParticleName().c_str());
            pmanager->AddDiscreteProcess(stepMaxProcess);
        }
    }
}

#endif
