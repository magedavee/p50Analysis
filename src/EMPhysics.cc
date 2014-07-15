// Unrestricted Use - Property of AECL
//
// EMPhysics.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Electromagnetic Processes (e-, e+, gamma)
// 	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "EMPhysics.hh"			// Specifies the file which contains the class structure

#include "G4EmStandardPhysics.hh"	// Specifies the classes which contain structures called upon in this class
#include "G4EmPenelopePhysics.hh"
#include "G4VUserPhysicsList.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include <iomanip>			// Specifies the classes which allow reading/writing to standard input/output
#include "G4ios.hh"

#include "globals.hh"			// Specifies class defining all global parameters and variable types

	// ****** Constructor ****** //
EMPhysics::EMPhysics(const G4String& name)
  :  G4VPhysicsConstructor(name)				// The function after colon indicates that when this object is "constructed", it also constructs a G4VPhysicsConstructor object as well
{
  em = new G4EmStandardPhysics(0,name);
  pen = new G4EmPenelopePhysics(0,name);
}

	// ****** Destructor ****** //
EMPhysics::~EMPhysics()
{
  delete em;
  delete pen;
}

	// ****** Process Specifier ****** //
void EMPhysics::ConstructProcess()		// Sets up the necessary processes for the desired particles
{
/*
  G4ProcessManager* process_manager = 0;

  // Gamma Physics

  process_manager = G4Gamma::Gamma()->GetProcessManager();	// Defines which particle these processes apply to

    // Gamma Conversion Process

    G4VProcess* gammaConversion = new G4GammaConversion();	// Creates new GammaConversion (pair production) process class
    process_manager->AddDiscreteProcess(gammaConversion);	// Registers the process to the particle's process manager

    // Compton Scattering Process

    G4VProcess* comptonScattering = new G4ComptonScattering();
    process_manager->AddDiscreteProcess(comptonScattering);

    // Photoelectric Effect Process

    G4VProcess* photoElectricEffect = new G4PhotoElectricEffect();
    process_manager->AddDiscreteProcess(photoElectricEffect);

  // Electron Physics

  process_manager = G4Electron::Electron()->GetProcessManager();	// The following is similar to the above...

    // Multiple Scattering Process

    G4VProcess* eMinusMultipleScattering = new G4eMultipleScattering();
    process_manager->AddProcess(eMinusMultipleScattering);
    process_manager->SetProcessOrdering(eMinusMultipleScattering,idxAlongStep,1);	// ... except that the process ordering must be defined as they will not be considered at every step position
    process_manager->SetProcessOrdering(eMinusMultipleScattering,idxPostStep,1);

    // Ionization Process

    G4VProcess* eMinusIonization = new G4eIonisation();
    process_manager->AddProcess(eMinusIonization);
    process_manager->SetProcessOrdering(eMinusIonization,idxAlongStep,2);
    process_manager->SetProcessOrdering(eMinusIonization,idxPostStep,2);

    // Bremsstrahlung Process

    G4VProcess* eMinusBremsstrahlung = new G4eBremsstrahlung();
    process_manager->AddProcess(eMinusBremsstrahlung);
    process_manager->SetProcessOrdering(eMinusBremsstrahlung,idxAlongStep,3);
    process_manager->SetProcessOrdering(eMinusBremsstrahlung,idxPostStep,3);

  // Positron Physics

  process_manager = G4Positron::Positron()->GetProcessManager();

    // Multiple Scattering Process

    G4VProcess* ePlusMultipleScattering = new G4eMultipleScattering();
    process_manager->AddProcess(ePlusMultipleScattering);
    process_manager->SetProcessOrdering(ePlusMultipleScattering,idxAlongStep,1);
    process_manager->SetProcessOrdering(ePlusMultipleScattering,idxPostStep,1);

    // Ionization Process

    G4VProcess* ePlusIonization = new G4eIonisation();
    process_manager->AddProcess(ePlusIonization);
    process_manager->SetProcessOrdering(ePlusIonization,idxAlongStep,2);
    process_manager->SetProcessOrdering(ePlusIonization,idxPostStep,2);

    // Bremsstrahlung Process

    G4VProcess* ePlusBremsstrahlung = new G4eBremsstrahlung();
    process_manager->AddProcess(ePlusBremsstrahlung);
    process_manager->SetProcessOrdering(ePlusBremsstrahlung,idxAlongStep,3);
    process_manager->SetProcessOrdering(ePlusBremsstrahlung,idxPostStep,3);

    // Annihiliation Process

    G4VProcess* ePlusAnnihilation = new G4eplusAnnihilation();
    process_manager->AddProcess(ePlusAnnihilation);
    process_manager->SetProcessOrdering(ePlusAnnihilation,idxAtRest);
    process_manager->SetProcessOrdering(ePlusAnnihilation,idxPostStep,4);
*/

//  em->ConstructProcess();		// Builds Standard EM Physics - not good
  pen->ConstructProcess();		// Builds Penelope Physics List - more universally applicable
}

// EOF
