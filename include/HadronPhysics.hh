// Unrestricted Use - property of AECL
//
// HadronPhysics.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Hadron/Ion Physics Processes
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef HadronPhysics_H			// Only carries out if object is undefined
#define HadronPhysics_H 1		// Defines object

#include "G4VPhysicsConstructor.hh"	// Specifies base class or parent class

#include "G4UHadronElasticProcess.hh"	// Specifies classes which contain structures called upon in this class
#include "G4HadronFissionProcess.hh"
#include "G4HadronCaptureProcess.hh"
#include "G4HadronElastic.hh"
#include "G4LElastic.hh"
#include "G4LFission.hh"
#include "G4LCapture.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4NeutronHPElastic.hh"			// *** High precision models were used due to suspected discrepancies with the other models ***
#include "G4NeutronHPorLElastic.hh"
#include "G4NeutronHPInelastic.hh"
#include "G4NeutronHPCapture.hh"
#include "G4NeutronHPFission.hh"
#include "G4LENeutronInelastic.hh"
#include "G4HENeutronInelastic.hh"
#include "G4NeutronHPThermalScattering.hh"
#include "G4NeutronHPThermalScatteringData.hh"
#include "G4NeutronHPElasticData.hh"
#include "G4NeutronHPorLElasticData.hh"
#include "G4NeutronHPCaptureData.hh"
#include "GdNeutronHPCapture.hh"

#include "G4ios.hh"			// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"			// Specifies class defining all global parameters and variable types

class HadronPhysicsQGSP_BERT_HP;	// Structures necessary for class definition

/* -------- Class Definition --------- */

class HadronPhysics : public G4VPhysicsConstructor	// This class inherits functions from G4VPhysicsConstructor
{
  public:	// Constructors and Destructors

    HadronPhysics(const G4String& name="Neutron");	// Constructor
    virtual ~HadronPhysics();				// Destructor

  public:	// Accessible Methods

    virtual void ConstructParticle() {;};		// Empty Function - why is this here?
    virtual void ConstructProcess();			// Function which defines the neutron-related processes in this simulation

  private:	// Member Data

    HadronPhysicsQGSP_BERT_HP* qgsp;
};

/* ----------------------------------- */

#endif						// End of the if clause

// EOF
