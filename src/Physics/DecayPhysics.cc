// Unrestricted Use - Property of AECL
//
// DecayPhysics.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Decay Physics Processes
// 	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "DecayPhysics.hh"		// Specifies the file which contains the class structure

#include "G4DecayPhysics.hh"		// Specifies the classes which contain structures called upon in this class
#include "G4RadioactiveDecayPhysics.hh"
#include "G4VUserPhysicsList.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include <iomanip>			// Specifies classes which allow reading/writing to standard input/output
#include "G4ios.hh"

#include "globals.hh"			// Specifies class defining all global parameters and variable types

	// ****** Constructor ****** //
DecayPhysics::DecayPhysics(const G4String& name1,const G4String& name2)
 :  G4VPhysicsConstructor(name1)		// The function after the colon indicates that when this object is "constructed", it also constructs a G4VPhysicsConstructor object as well
{
  decay = new G4DecayPhysics(name1);
  radDecay = new G4RadioactiveDecayPhysics(name2);
}

	// ****** Destructor ****** //
DecayPhysics::~DecayPhysics()
{
  delete decay;
  delete radDecay;
}

	// ****** Process Specifier ****** //
void DecayPhysics::ConstructProcess()		// Sets up the necessary process for the desired particles
{
/*
  G4ProcessManager* process_manager = 0;
  theParticleIterator->reset();
  while((*theParticleIterator)())
  {
    G4ParticleDefinition* particle = theParticleIterator->value();
    process_manager = particle->GetProcessManager();
    if(decayProcess->IsApplicable(*particle))
    {
      process_manager->AddProcess(decayProcess);

	// Set process ordering to PostStepDoIt and AtRestDoIt

      process_manager->SetProcessOrdering(decayProcess,idxPostStep);
      process_manager->SetProcessOrdering(decayProcess,idxAtRest);
    }
  }
*/

  decay->ConstructProcess();		// Sets up decay processes
  radDecay->ConstructProcess();		// Sets up radioactive decay processes
}

// EOF
