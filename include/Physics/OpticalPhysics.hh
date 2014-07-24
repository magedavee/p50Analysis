// Unrestricted Use - Property of AECL
//
// OpticalPhysics.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Optical Photon Physics Processes
// 	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef OpticalPhysics_H		// Only carries out if object is undefined
#define OpticalPhysics_H 1		// Defines object

#include "globals.hh"			// Includes definition for global constants and variables types
#include "G4ios.hh"			// Includes class allowing reading/writing into the standard input/output

#include "G4VPhysicsConstructor.hh"	// Includes the base physics construction class

#include "G4PhotoElectricEffect.hh"	// Includes general gamma process libraries
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"

#include "G4OpAbsorption.hh"		// Includes some optical photon physics libraries for generated optical photons
#include "G4OpRayleigh.hh"
#include "G4OpBoundaryProcess.hh"

#include "G4Scintillation.hh"		// Includes processes which generate optical photons
#include "G4EmSaturation.hh"
#include "G4Cerenkov.hh"

	// Class Definition
class OpticalPhysics : public G4VPhysicsConstructor
{
  public:

    OpticalPhysics(const G4String& name = "OpticalPhoton");	// Constructor - default name for physics process is "Electronmagnetic"
    virtual ~OpticalPhysics();					// Destructor

  public:	// Member Functions

    virtual void ConstructParticle() {;};	// Emtpy Function which is needed to use Physics Constructor base class
    virtual void ConstructProcess();		// Function which defines the EM processes to be modelled in this simulation

  private:	// Member Data
};

#endif					// End of the if clause

// EOF
