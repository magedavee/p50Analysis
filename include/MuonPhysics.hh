/*
// Unrestricted Use - property of AECL
//
// MuonPhysics.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Muon Physics Processes (mu+, mu-)
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef MuonPhysics_H			// Only carries out if object is undefined
#define MuonPhysics_H 1			// Defines object

#include "G4VPhysicsConstructor.hh"	// Specifies base class or parent class

#include "G4MuBremsstrahlung.hh"	// Specifies classes which contain structures called upon in this class
#include "G4MuIonisation.hh"
#include "G4MuMultipleScattering.hh"
#include "G4MuPairProduction.hh"
#include "G4MuNuclearInteraction.hh"

#include "G4ios.hh"			// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"			// Specifies class defining all global parameters and variable types


class MuonPhysics : public G4VPhysicsConstructor	// This class inherits functions from G4VPhysicsConstructor
{
  public:	// Constructors and Destructors

    MuonPhysics(const G4String& name="Muon");		// Constructor
    virtual ~MuonPhysics();				// Destructor

  public:	// Accessible Methods

    virtual void ConstructParticle() {;};		// Empty Function - why is this here?
    virtual void ConstructProcess();			// Function which defines the neutron-related processes in this simulation
};


#endif
*/