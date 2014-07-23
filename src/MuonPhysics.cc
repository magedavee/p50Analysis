/*
// Unrestricted Use - Property of AECL
//
// MuonPhysics.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Muon Physics Processes (mu+, mu-)
// 	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "MuonPhysics.hh"		// Specifies the file which contains the class structure

#include "G4VUserPhysicsList.hh"	// Specifies the classes which contain structures called upon in this class
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"

#include <iomanip>			// Specifies the classes which allow reading/writing to standard input/output
#include "G4ios.hh"

#include "globals.hh"			// Specifies class defining all global parameters and variable types

	// ****** Constructor ****** //
MuonPhysics::MuonPhysics(const G4String& name)
 :  G4VPhysicsConstructor(name)		// The function after the colon indicates that when this object is "constructed", it also constructs a G4VPhysicsConstructor object as well
{;}

	// ****** Destructor ****** //
MuonPhysics::~MuonPhysics()
{;}

	// ****** Process Specifier ****** //
void MuonPhysics::ConstructProcess()			// Sets up the necessary process for the desired particles
{
  G4ProcessManager* process_manager = 0;

  // Muon Plus Physics

  process_manager = G4MuonPlus::MuonPlus()->GetProcessManager();	// Defines which process manager is being modified

  G4MuMultipleScattering* muMScProcess = new G4MuMultipleScattering();
  process_manager->AddProcess(muMScProcess, -1, 1, 1);

  G4MuIonisation* muIonProcess = new G4MuIonisation();
  process_manager->AddProcess(muIonProcess, -1, 2, 2);

  G4MuBremsstrahlung* muBremsProcess = new G4MuBremsstrahlung();
  process_manager->AddProcess(muBremsProcess, -1, 3, 3);

  G4MuPairProduction* muPairProcess = new G4MuPairProduction();
  process_manager->AddProcess(muPairProcess, -1, 4, 4);

  G4MuNuclearInteraction* muNuclearProcess = new G4MuNuclearInteraction();
  process_manager->AddDiscreteProcess(muNuclearProcess);

  // Muon Minus Physics

  process_manager = G4MuonMinus::MuonMinus()->GetProcessManager();	// Defines which process manager is being modified

  muMScProcess = new G4MuMultipleScattering();
  process_manager->AddProcess(muMScProcess, -1, 1, 1);

  muIonProcess = new G4MuIonisation();
  process_manager->AddProcess(muIonProcess, -1, 2, 2);

  muBremsProcess = new G4MuBremsstrahlung();
  process_manager->AddProcess(muBremsProcess, -1, 3, 3);

  muPairProcess = new G4MuPairProduction();
  process_manager->AddProcess(muPairProcess, -1, 4, 4);

  muNuclearProcess = new G4MuNuclearInteraction();
  process_manager->AddDiscreteProcess(muNuclearProcess);
}

// EOF
*/