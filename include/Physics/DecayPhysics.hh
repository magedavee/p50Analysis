// Unrestricted Use - property of AECL
//
// DecayPhysics.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Decay Physics Processes
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef DecayPhysics_H			// Only carries out if object is undefined
#define DecayPhysics_H 1		// Defines object

#include "G4VPhysicsConstructor.hh"	// Specifies base class or parent class

#include "G4Decay.hh"			// Specifies classes which contain structures called upon in this class
#include "G4RadioactiveDecay.hh"

#include "G4ios.hh"			// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"			// Specifies class defining all global parameters and variable types

class G4DecayPhysics;			// Structures necessary for class definition
class G4RadioactiveDecayPhysics;

/* -------- Class Definition --------- */

class DecayPhysics : public G4VPhysicsConstructor	// This class inherits functions from G4VPhysicsConstructor
{
  public:	// Constructors and Destructors

    DecayPhysics(const G4String& name1="Decay",const G4String& name2="Radioactive Decay");	// Constructor
    virtual ~DecayPhysics();									// Destructor

  public:	// Accessible Methods

    virtual void ConstructParticle() {;};		// Empty Function - why is this here?
    virtual void ConstructProcess();			// Function which defines the decay process for this simulation

  private:	// Member Data

    G4DecayPhysics* decay;
    G4RadioactiveDecayPhysics* radDecay;
};

/* ----------------------------------- */

#endif					// End of the if clause

// EOF
