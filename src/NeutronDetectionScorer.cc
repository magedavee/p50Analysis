// Unrestricted Use - Property of AECL
//
// NeutronDetectionScorer.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Neutron Detection Scorer
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "NeutronDetectionScorer.hh"		// Specifies the file which contains the class structure

#include "NeutronHit.hh"					// Specifies user-defined classes which are called upon in this class
#include "LogSession.hh"

#include "G4Track.hh"					// Specifies classes which contain structures called upon in this class
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4THitsMap.hh"
#include "G4ParticleTypes.hh"
#include "G4VSolid.hh"

#include "RootIO.hh"

#include "G4ios.hh"					// Specifies the classes which allow reading/writing to standard input/output

#include "globals.hh"					// Specifies class defining all global constants and variable types

	// ****** Constructor ****** //
NeutronDetectionScorer::NeutronDetectionScorer(G4String HCname)
 : G4VPrimitiveScorer(HCname)
{
  HCIDNeut = -1;
  nHit_collection = 0;
}

	// ****** Destructor ****** //
NeutronDetectionScorer::~NeutronDetectionScorer()
{
  delete nHit_collection;
}

	// ****** Initialize Event ****** //
void NeutronDetectionScorer::Initialize(G4HCofThisEvent* HCE)
{
  G4String name = GetMultiFunctionalDetector()->GetName();
  nHit_collection = new NeutronHitsCollection(name,primitiveName);
  if(HCIDNeut<0)
  {
    HCIDNeut = G4SDManager::GetSDMpointer()->GetCollectionID(nHit_collection);
  }
  HCE->AddHitsCollection(HCIDNeut,nHit_collection);
}

	// ****** Process Event Hit ****** //
G4bool NeutronDetectionScorer::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
	// Specifies criteria for what will be counted as a hit for a collection, called per step
  if(aStep->GetTrack()->GetDefinition() == G4Neutron::NeutronDefinition())
  {
	// Generate new hit and initialize basic values
    NeutronHit* aHit = new NeutronHit();
    G4bool isNew = true;
    aHit->SetTrackID(aStep->GetTrack()->GetTrackID());
    aHit->SetEnergy(aStep->GetPreStepPoint()->GetKineticEnergy());
    aHit->SetInitialPosition(aStep->GetPreStepPoint()->GetPosition());
    aHit->SetEntryTime(aStep->GetPreStepPoint()->GetLocalTime());
    aHit->SetTrackTime(aStep->GetPostStepPoint()->GetLocalTime());
    aHit->SetThermalizationTime(aStep->GetPostStepPoint()->GetLocalTime());
    aHit->SetLeft(false);
    aHit->SetCaptured(false);
    aHit->SetForeign(false);

	// Record an entry track if particle generationx LOGICAL volume is not the same the current LOGICAL volume
    if(!(aStep->GetTrack()->GetLogicalVolumeAtVertex() == aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()))
    {
      aHit->SetForeign(true);	// Set entry flag

	// Record the zenith angle if current step is the first step in the sensitive volume
      if(aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary)
      {
        aHit->SetZenithAngle(std::fabs((aStep->GetPreStepPoint()->GetMomentumDirection()).angle(G4ThreeVector(0.,-1.,0.))));
      }
    }

	// Record a neutron capture if the end of step process is nCapture
      if(aStep->GetPostStepPoint()->GetProcessDefinedStep() != 0 && aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "nCapture")
    {
  
      aHit->SetCaptured(true);	// Set capture flag
      aHit->SetFinalPosition(aStep->GetPostStepPoint()->GetPosition());

	// Record capture gamma production if the capture event produces secondary gamma particles
      G4TrackVector* secondaries = aStep->GetSecondary();
      if(!secondaries->empty())
      {
        G4int nGamma = 0;
        std::stringstream stream;
        std::vector<G4Track*>::iterator itrTrack = secondaries->begin();
        G4double totGammaE = 0.0;
        for( ; itrTrack != secondaries->end(); itrTrack++)
        {
	  //	  G4cerr<<"Ncap secondaries:\t"<<aStep->GetTrack()->GetDefinition()->GetParticleName()<<"\t"<<(*itrTrack)->GetDefinition()->GetParticleName()<<"\t"<<(*itrTrack)->GetDefinition()->GetAtomicNumber()<<G4endl;
	  if((*itrTrack)->GetDefinition()->GetAtomicNumber()==64)  {
	    RootIO::GetInstance()->GetEvent()->fNcap = 64;
	    //  G4cerr<<"NCAP 64"<<endl;
	  }
	  if((*itrTrack)->GetDefinition()->GetAtomicNumber()==1)  {
	    RootIO::GetInstance()->GetEvent()->fNcap = 1;
	    //	    G4cerr<<"NCAP 1"<<endl;
	  }
          if((*itrTrack)->GetDefinition() == G4Gamma::GammaDefinition()) { stream << (*itrTrack)->GetKineticEnergy() << std::endl; totGammaE += (*itrTrack)->GetKineticEnergy(); nGamma++; }
        }
        stream << "*** " << totGammaE << std::endl;
        aHit->SetGammasGenerated(nGamma);
      }
    }

	// Record an escape track if the particle leaves the sensitive volume
    if(aStep->GetPostStepPoint()->GetStepStatus() == fGeomBoundary)
    {
      aHit->SetFinalPosition(aStep->GetPostStepPoint()->GetPosition());
      aHit->SetLeft(true);	// Set escape flag
    }

	// Stop tallying thermalization time if kinetic energy of neutron is < 1 eV
    if(aStep->GetPreStepPoint()->GetKineticEnergy() < 1.0*eV) { aHit->SetThermalizationTime(-1.*s); }

	// Compare produced hit to all stored hits and combine if any are equal
    for(int i = 0; i < nHit_collection->entries(); i++)
    {
	// Comparative operator checks track ID
      if(*((*nHit_collection)[i]) == *aHit)	
      {
        *((*nHit_collection)[i]) += *aHit;
        isNew = false;
        break;
      }
    }

	// If comparison does not find an equality, register a new hit into the collection
    if(isNew)
    {
	// Set thermalization time to 0 s for new entries
      if(aHit->GetThermalizationTime() < 0.*s) { aHit->SetThermalizationTime(0.*s); }
      nHit_collection->insert(aHit);
    }
    return true;
  }
  return false;
}

	// ****** Terminate Event ****** //
void NeutronDetectionScorer::EndOfEvent(G4HCofThisEvent*)
{
	// Additional hit processing at end-of-event can be placed here
  HCIDNeut = -1;
  nHit_collection = 0;
}

// EOF
