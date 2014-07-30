/*
// Unrestricted Use - Property of AECL
//
// HadronPhysics.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Hadron/Ion Physics Processes
// 	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "HadronPhysics.hh"		// Specifies the file which contains the class structure

#include "HadronPhysicsQGSP_BERT_HP.hh" // Specifies the classes which contain structures called upon in this class
#include "G4VUserPhysicsList.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4Neutron.hh"

#include <iomanip>			// Specifies the classes which allow reading/writing to standard input/output
#include "G4ios.hh"

#include "globals.hh"			// Specifies class defining all global parameters and variable types

	// ****** Constructor ****** //
HadronPhysics::HadronPhysics(const G4String& name)
 :  G4VPhysicsConstructor(name)		// The function after the colon indicates that when this object is "constructed", it also constructs a G4VPhysicsConstructor object as well
{
  qgsp = new HadronPhysicsQGSP_BERT_HP(name);
}

	// ****** Destructor ****** //
HadronPhysics::~HadronPhysics()
{ 
  delete qgsp;
}

	// ****** Process Specifier ****** //
void HadronPhysics::ConstructProcess()			// Sets up the necessary process for the desired particles
{
  qgsp->ConstructProcess();	// QGSP constructed first to allow replacing of the models in this builder

	// This section edited out to implement QGSP_BERT_HP

  G4ProcessManager* process_manager = 0;

  // Neutron Physics

  process_manager = G4Neutron::Neutron()->GetProcessManager();	// Defines which process manager is being modified

  process_manager->RemoveProcess(2);

    // Elastic Process

    G4UHadronElasticProcess* neutronElasticProcess = new G4UHadronElasticProcess();		// Creates new process object
    G4NeutronHPThermalScatteringData* thermalScatteringData = new G4NeutronHPThermalScatteringData();
    neutronElasticProcess->AddDataSet(thermalScatteringData);
    G4NeutronHPThermalScattering* thermalScatteringModel = new G4NeutronHPThermalScattering();
    thermalScatteringModel->SetMaxEnergy(4.0*eV);
    neutronElasticProcess->RegisterMe(thermalScatteringModel);					// Add thermal scattering for neutrons under 4 eV
    G4NeutronHPElasticData* neutronElasticData = new G4NeutronHPElasticData();
    neutronElasticProcess->AddDataSet(neutronElasticData);
    G4NeutronHPorLElastic* neutronElasticModel = new G4NeutronHPorLElastic();			// Creates new model object
    neutronElasticModel->SetMinEnergy(4.0*eV);
    neutronElasticModel->SetMaxEnergy(20.*MeV);
    neutronElasticProcess->RegisterMe(neutronElasticModel);					// Registers model into the process (Standard elastic model)
    G4HadronElastic* hadronElasticModel = new G4HadronElastic();
    hadronElasticModel->SetMinEnergy(19.*MeV);
    neutronElasticProcess->RegisterMe(hadronElasticModel);					// Add high energy hadron model for neutron over 20 MeV
    process_manager->AddDiscreteProcess(neutronElasticProcess);					// Adds newly defined process to the particle process manager

    // Inelastic Process

//    G4NeutronInelasticProcess* neutronInelasticProcess = new G4NeutronInelasticProcess();	// Creates new process object
//    G4LENeutronInelastic* neutronLEPModel = new G4LENeutronInelastic();				// Creates new model objects
//    G4HENeutronInelastic* neutronHEPModel = new G4HENeutronInelastic();
//    G4NeutronHPInelastic* neutronInelasticModel = new G4NeutronHPInelastic();
//    neutronLEPModel->SetMaxEnergy(9.0*GeV);							// Sets energy ranges for models
//    neutronHEPModel->SetMinEnergy(20.0*GeV);
//    neutronInelasticProcess->RegisterMe(neutronLEPModel);					// Registers models into the process
//    neutronInelasticProcess->RegisterMe(neutronHEPModel);
//    neutronInelasticProcess->RegisterMe(neutronInelasticModel);
//    process_manager->AddDiscreteProcess(neutronInelasticProcess);				// Adds newly defined process to the particle process manager

    // Fission Process

//    G4HadronFissionProcess* neutronFissionProcess = new G4HadronFissionProcess();		// Creates new process object
//    G4LFission* neutronFissionModel = new G4LFission();						// Creates new model object
//    G4NeutronHPFission* neutronFissionModel = new G4NeutronHPFission();
//    neutronFissionProcess->RegisterMe(neutronFissionModel);					// Registers model into the process
//    process_manager->AddDiscreteProcess(neutronFissionProcess);					// Adds newly defined process to the particle process manager

    // Capture Process

    G4HadronCaptureProcess* neutronCaptureProcess = new G4HadronCaptureProcess();		// Creates new process object
    G4NeutronHPCaptureData* neutronCaptureData = new G4NeutronHPCaptureData();
    neutronCaptureProcess->AddDataSet(neutronCaptureData);
    GdNeutronHPCapture* gdCaptureModel = new GdNeutronHPCapture();
    neutronCaptureProcess->RegisterMe(gdCaptureModel);
    G4LCapture* neutronCaptureModel = new G4LCapture();						// Creates new model object
    neutronCaptureModel->SetMinEnergy(19.9*MeV);
    neutronCaptureModel->SetMaxEnergy(20.0*TeV);
    neutronCaptureProcess->RegisterMe(neutronCaptureModel);					// Registers model into the process
    process_manager->AddDiscreteProcess(neutronCaptureProcess);					// Adds newly defined process to the particle process manager
}

// EOF
*/
