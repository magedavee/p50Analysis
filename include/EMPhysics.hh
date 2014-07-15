// Unrestricted Use - Property of AECL
//
// EMPhysics.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Electromagnetic Physics Processes (e-, e+, gamma)
// 	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef EMPhysics_H			// Only carries out if object is undefined
#define EMPhysics_H 1			// Defines object

#include "G4VPhysicsConstructor.hh"	// Specifies base class or parent class

#include "G4PhotoElectricEffect.hh"	// Specifies the classes which contain structures called upon in this class
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eMultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eplusAnnihilation.hh"

#include "G4ios.hh"			// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"			// Specifies class defining all global parameters and variables types

class G4EmStandardPhysics;		// Structures necessary for class definition
class G4EmPenelopePhysics;

/* -------- Class Definition --------- */

class EMPhysics : public G4VPhysicsConstructor		// This class inherits functions from G4VPhysicsConstructor
{
  public:	// Constructors and Destructors

    EMPhysics(const G4String& name = "Electromagnetic");	// Constructor
    virtual ~EMPhysics();					// Destructor

  public:	// Accessible Methods

    virtual void ConstructParticle() {;};	// Empty Function - why is this here?
    virtual void ConstructProcess();		// Function which defines the EM processes for this simulation

  private:	// Member Data

    G4EmStandardPhysics* em;
    G4EmPenelopePhysics* pen;
};

/* ----------------------------------- */

#endif					// End of the if clause

// EOF
