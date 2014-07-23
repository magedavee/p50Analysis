//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PhysicsList.hh"

#include <G4EmLivermorePhysics.hh>
#include <G4EmPenelopePhysics.hh>
#include <G4SystemOfUnits.hh>

#include <G4Gamma.hh>
#include <G4Electron.hh>
#include <G4Positron.hh>

#include <G4LossTableManager.hh>
#include <G4EmConfigurator.hh>
#include <G4UnitsTable.hh>

#include <G4ProcessManager.hh>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList_495::PhysicsList_495(bool usePenelope) : G4VModularPhysicsList() {
    
    G4LossTableManager::Instance();
    defaultCutValue = 1.*um;
    cutForGamma     = defaultCutValue;
    cutForElectron  = 0.5*defaultCutValue;
    cutForPositron  = defaultCutValue;
    
    SetVerboseLevel(1);
    
    // EM physics
    if(usePenelope) {
        emName = G4String("Penelope");  
        emPhysicsList = new G4EmPenelopePhysics();
    } else {
        emName = G4String("Livermore");  
        emPhysicsList = new G4EmLivermorePhysics();
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList_495::~PhysicsList_495() {
    delete emPhysicsList;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
////////////////////////////////////////////////////////////////////////////
// Construct Particles /////////////////////////////////////////////////////

void PhysicsList_495::ConstructParticle() {     
    emPhysicsList->ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList_495::ConstructProcess() {
    // transportation process
    AddTransportation();
    // electromagnetic physics list
    emPhysicsList->ConstructProcess();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/*
 v oid PhysicsList_Livermore::AddStepMax()                                        *
 {
     // Step limitation seen as a process
     stepMaxProcess = new StepMax();
        
        theParticleIterator->reset();
        while ((*theParticleIterator)()){
            G4ParticleDefinition* particle = theParticleIterator->value();
        G4ProcessManager* pmanager = particle->GetProcessManager();
        if (stepMaxProcess->IsApplicable(*particle) && pmanager) {
            pmanager->AddDiscreteProcess(stepMaxProcess);
 }
 }
 }
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList_495::SetCuts() {
    
    if (verboseLevel >0) {
        G4cout << "PhysicsList::SetCuts:";
        G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
    }
    
    if(emName == "Livermore")
        G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(250*eV, 1*GeV);
    else if(emName == "Penelope")
        G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(100*eV, 1*GeV);
    
    // set cut values for gamma at first and for e- second and next for e+,
        // because some processes for e+/e- need cut values for gamma
        SetCutValue(cutForGamma, "gamma");
        SetCutValue(cutForElectron, "e-");
        SetCutValue(cutForPositron, "e+");
        
        if (verboseLevel>0) DumpCutValuesTable();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList_495::SetCutForGamma(G4double cut) {
    cutForGamma = cut;
    SetParticleCuts(cutForGamma, G4Gamma::Gamma());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList_495::SetCutForElectron(G4double cut) {
    cutForElectron = cut;
    SetParticleCuts(cutForElectron, G4Electron::Electron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList_495::SetCutForPositron(G4double cut) {
    cutForPositron = cut;
    SetParticleCuts(cutForPositron, G4Positron::Positron());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


/*
#include "PhysicsList.hh" 			// Specifies the file which contains the class structure

#include "HadronPhysics.hh"			// Specifies user-defined classes which are called upon in this class
#include "DecayPhysics.hh"
#include "EMPhysics.hh"
#include "OpticalPhysics.hh"

#include "G4ParticleTypes.hh"			// These are the GEANT4 classes necessary to define the particles
#include "G4ParticleTable.hh"			//	|
#include "G4ParticleDefinition.hh"		//	|
#include "G4ParticleWithCuts.hh"		//	|
#include "G4BaryonConstructor.hh"		//	|
#include "G4BosonConstructor.hh"		//	|
#include "G4IonConstructor.hh"			//	|
#include "G4LeptonConstructor.hh"		//	|
#include "G4MesonConstructor.hh"		//	|
#include "G4ShortLivedConstructor.hh"		//	V

#include "G4ProcessManager.hh"			// These are the GEANT4 classes necessary to define the processes
#include "G4ProcessVector.hh"			//	|
#include "G4RunManager.hh"			//	V

#include "G4VUserPhysicsList.hh"		// Specifies all the classes which contain structures called upon in this class
#include "G4UnitsTable.hh"

#include "G4ios.hh"				// Specifies the classes which allow reading/writing into the standard input/output
#include <iomanip>

#include "globals.hh"				// Specifies class defining all global parameters and variable types (double, int, string, etc.)

	// ****** Constructor ****** //
PhysicsList::PhysicsList()		// Instantiation of object and initialization of global variables, defined by corresponding header file
 :  G4VModularPhysicsList()
{
  G4double newCutValue;
  SetDefaultCutValue(newCutValue = 1.0*mm);	// Assumed that this is the standard cut value, subject to change
  SetVerboseLevel(1);				// Assumed that this Modular class does not have a default verbose level
  DisableCheckParticleList();			// Disable additional function which checks particle list (DO NOT REMOVE!! - causes segmentation fault otherwise)

	// Instantiating individual physics sets
  G4String name, title;
  HadronPhysics* neutron_physics = new HadronPhysics(name = "Neutron");
  DecayPhysics* decay_physics = new DecayPhysics(name = "Decay", title = "Radioactive Decay");
  EMPhysics* em_physics = new EMPhysics(name = "EM");
  OpticalPhysics* optical_physics = new OpticalPhysics(name = "Optical Photon");

	// Registers the sets into the physics list
  RegisterPhysics(neutron_physics);
  RegisterPhysics(decay_physics);
  RegisterPhysics(em_physics);
  RegisterPhysics(optical_physics);
} */