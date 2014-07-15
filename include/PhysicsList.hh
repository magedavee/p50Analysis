// Unrestricted Use - Property of AECL
//
// PhysicsList.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Physics Process Specifications
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef PhysicsList_H			// Only carries out following if the object is undefined
#define PhysicsList_H 1			// Defines file

#include "G4VModularPhysicsList.hh"  	// Specifies base class or parent class

#include "globals.hh"			// Specifies class defining all global parameters and variable types

/* -------- Class Definition --------- */

class PhysicsList: public G4VModularPhysicsList		// This class inherits functions from G4VModularPhysicsList
{
  public:	// Constructors and Destructors

    PhysicsList();		// Constructor
    virtual ~PhysicsList();	// Destructor

  public:	// Accessible Methods

    virtual void ConstructParticle();	// Particles are constructed here in this simulation - every particle type is generated to avoid complications later
    virtual void SetCuts();		// Particle cuts are set here in this simulation

	// *NOTE: ConstructProcess() method not needed since G4VModularPhysicsList already has one which calls AddTransportation() and ConstructProcess() of registered physics lists.*
};

/* ----------------------------------- */

#endif					// End of the if clause

// EOF
