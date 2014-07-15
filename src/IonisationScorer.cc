// Unrestricted Use - Property of AECL
//
// IonisationScorer.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Ionizing Radiation Scorer
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "IonisationScorer.hh"		// Specifies the file which contains the class structure

#include "IonisationHit.hh"					// Specifies user-defined classes which are called upon in this class
#include "LogSession.hh"
#include "RootIO.hh"

#include "G4Track.hh"					// Specifies classes which contain structures called upon in this class
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4THitsMap.hh"
#include "G4ParticleTypes.hh"
#include "G4VSolid.hh"
#include "G4RunManager.hh"	
#include "RunAction.hh"

#include "G4ios.hh"					// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"					// Specifies class defining all global constants and variable types

	// ****** Constructor ****** //
IonisationScorer::IonisationScorer(G4String HCname)
 : G4VPrimitiveScorer(HCname)
{
  HCIDIon = -1;
  ionise_collection = 0;
}

	// ****** Destructor ****** //
IonisationScorer::~IonisationScorer()
{
  delete ionise_collection;
}

	// ****** Initialize Event ****** //
void IonisationScorer::Initialize(G4HCofThisEvent* HCE)
{
  G4String name = GetMultiFunctionalDetector()->GetName();
  ionise_collection = new IonisationHitsCollection(name,primitiveName);
  if(HCIDIon<0)
  {
    HCIDIon = G4SDManager::GetSDMpointer()->GetCollectionID(ionise_collection);
  }
  HCE->AddHitsCollection(HCIDIon,ionise_collection);
}

	// ****** Process Event Hit ****** //
G4bool IonisationScorer::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{ 
  RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
	// Specifies criteria for what will be counted as a hit for a collection, called per step
  if(aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.0 || aStep->GetTrack()->GetDefinition() == G4Neutron::NeutronDefinition() || aStep->GetTrack()->GetDefinition() == G4Gamma::GammaDefinition())
  {
	// Check if particle is a neutron passing through veto panels - reject hit if true
    if(aStep->GetTrack()->GetDefinition() == G4Neutron::NeutronDefinition() && aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName().find("Veto Panel") != std::string::npos)
    { return false; }

	 // Generate new hit and initialize basic values
    IonisationHit* aHit = new IonisationHit();
    aHit->SetVolume(aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName());
    aHit->SetTrackID(aStep->GetTrack()->GetTrackID());

	// Record total energy deposit if particle is charged (e-, e+, etc.)
    if(aStep->GetTrack()->GetDefinition()->GetPDGCharge() != 0.0)
    {
      aHit->SetEnergyDeposit(aStep->GetTotalEnergyDeposit());
    }
/* Need to implement direct energy deposit by gamma in case of high production threshold for e- */
 	
	// If step leaves volume, record the particle energy at point of exit as the escape energy
    if(aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary)
    { 
      if(aStep->GetTrack()->GetDefinition() == G4Positron::PositronDefinition()) { aHit->SetEnergyEscaped(aStep->GetPostStepPoint()->GetTotalEnergy()); }
      else { aHit->SetEnergyEscaped(aStep->GetPostStepPoint()->GetKineticEnergy()); }
      aHit->SetLeft(aStep->GetTrack()->GetTrackID()); 	// Set escape flag
    }
   
	// Treats all ionisation hits within the same event in the same scorer object to be cumulative - saves memory
    if(ionise_collection->entries() != 0)
    { 
	// Checks existing entry for escaped energy and subtracts appropriate amount if particle is re-entering the sensitive volume
      G4double escape = (*ionise_collection)[0]->GetEnergyEscaped(); 
      if(escape > 0.*MeV && aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary)
      { 
        if(aStep->GetTrack()->GetDefinition() == G4Positron::PositronDefinition()) { aHit->SetEnergyEscaped(-(aStep->GetPreStepPoint()->GetTotalEnergy())); }
        else { aHit->SetEnergyEscaped(-(aStep->GetPreStepPoint()->GetKineticEnergy())); }
      }
 
	// Comparative operator checks volume name and combines hit details if the hits have the same volume name
      // if(*((*ionise_collection)[0]) == *aHit) { *((*ionise_collection)[0]) += *aHit; }
    }
    else { ionise_collection->insert(aHit); }
    return true;
  }
  return false;
}

	// ****** Terminate Event ****** //
void IonisationScorer::EndOfEvent(G4HCofThisEvent*)
{
	// Additional hit processing at end-of-event can be placed here
  HCIDIon = -1;
  ionise_collection = 0;
}

// EOF
