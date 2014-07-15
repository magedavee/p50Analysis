// Unrestricted Use - Property of AECL
//
// PhysicsList.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Physics Process Specifications
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

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
}

	// ****** Destructor ****** //
PhysicsList::~PhysicsList()		// Removes any variables which may interfere with next instance of object and to save memory
{;}

	// ****** Particle Specifier ****** //
void PhysicsList::ConstructParticle()	// Defines particles to be used in this simulation
{
  // Constructs all particles just to be safe though most are unused in this simulation

	// Barions
  G4BaryonConstructor* baryon_constructor = new G4BaryonConstructor();
  baryon_constructor->ConstructParticle();
  delete baryon_constructor;

	// Bosons
  G4BosonConstructor* boson_constructor = new G4BosonConstructor();
  boson_constructor->ConstructParticle();
  delete boson_constructor;

	// Ions
  G4IonConstructor* ion_constructor = new G4IonConstructor();
  ion_constructor->ConstructParticle();
  delete ion_constructor;

	// Leptons
  G4LeptonConstructor* lepton_constructor = new G4LeptonConstructor();
  lepton_constructor->ConstructParticle();
  delete lepton_constructor;

	// Mesons
  G4MesonConstructor* meson_constructor = new G4MesonConstructor();
  meson_constructor->ConstructParticle();
  delete meson_constructor;

	// Short Lived
  G4ShortLivedConstructor* short_lived_constructor = new G4ShortLivedConstructor();
  short_lived_constructor->ConstructParticle();
  delete short_lived_constructor;
}

	// ****** Change Production Cuts ****** //
void PhysicsList::SetCuts()		// Function defines the cut values for each particle 
{
  if(verboseLevel > 0)
  {
    G4cout << "Cuts have been set to: " << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }

	// Arbitrarily chosen to be the default cut value for now
  SetCutValue(GetDefaultCutValue(),"gamma");
  SetCutValue(GetDefaultCutValue(),"e-");
  SetCutValue(GetDefaultCutValue(),"e+");

  if(verboseLevel > 0)
  {
    DumpCutValuesTable();
  }
}

// EOF
