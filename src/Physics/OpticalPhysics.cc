// Unrestricted Use - Property of AECL
//
// OpticalPhysics.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Optical Photon Physics Processes
// 	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "OpticalPhysics.hh"		// Specifies the file which contains the class structure

#include "G4VUserPhysicsList.hh"	// Specifies the classes which contain structures called upon in this class
#include "G4ProcessManager.hh"
#include "G4LossTableManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include <iomanip>			// Specifies the classes which allow reading/writing to standard input/output
#include "G4ios.hh"

#include "globals.hh"			// Specifies class defining all global parameters and variable types

	// ****** Constructor ****** //
OpticalPhysics::OpticalPhysics(const G4String& name)
  :  G4VPhysicsConstructor(name)		// The function after colon indicates that when this object is "constructed", it also constructs a G4VPhysicsConstructor object as well
{;}

	// ****** Destructor ****** //
OpticalPhysics::~OpticalPhysics()
{;}

	// ****** Process Specifier ****** //
void OpticalPhysics::ConstructProcess()				// Sets up the necessary process for the desired particles
{
    /*
  // Optical Photon Physics

  G4ProcessManager* process_manager = G4OpticalPhoton::OpticalPhoton()->GetProcessManager();		// Defines which process manager is being modified

    // Photoelectric Effect Process

    G4VProcess* photoElectricEffect = new G4PhotoElectricEffect();
    process_manager->AddDiscreteProcess(photoElectricEffect);

    // Optical Absorption Process

    G4VProcess* opAbsorption = new G4OpAbsorption();
    process_manager->AddDiscreteProcess(opAbsorption);

    // Rayleigh Scattering Process

    G4VProcess* rayleighScattering = new G4OpRayleigh();
    process_manager->AddDiscreteProcess(rayleighScattering);

    // Optical Boundary Process

    G4VProcess* opBoundary = new G4OpBoundaryProcess();
    process_manager->AddDiscreteProcess(opBoundary);

  // Scintillation Process - generation of optical photons

  process_manager = 0;

  G4Scintillation* scintProcess = new G4Scintillation("Scintillation");
  scintProcess->SetTrackSecondariesFirst(true);
  scintProcess->SetScintillationYieldFactor(1.0);

  G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
  scintProcess->AddSaturation(emSaturation);

  theParticleIterator->reset();

  while((*theParticleIterator)())
  {
    G4ParticleDefinition* particle = theParticleIterator->value();
    process_manager = particle->GetProcessManager();
    if(scintProcess->IsApplicable(*particle))
    {
      process_manager->AddProcess(scintProcess);
      process_manager->SetProcessOrderingToLast(scintProcess,idxAtRest);
      process_manager->SetProcessOrderingToLast(scintProcess,idxPostStep);
    }
  }

  // Cerenkov Process - generation of optical photons

  process_manager = 0;

  G4Cerenkov* cerenkovProcess = new G4Cerenkov("Cerenkov");
  cerenkovProcess->SetTrackSecondariesFirst(true);
  cerenkovProcess->SetMaxNumPhotonsPerStep(300);

  theParticleIterator->reset();

  while((*theParticleIterator)()) {
    G4ParticleDefinition* particle = theParticleIterator->value();
    process_manager = particle->GetProcessManager();
    if(cerenkovProcess->IsApplicable(*particle)) {
      process_manager->AddProcess(cerenkovProcess);
      process_manager->SetProcessOrdering(cerenkovProcess,idxPostStep);
    }
  }
  */
}

// EOF
