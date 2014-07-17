// Unrestricted Use - Property of AECL
//
// TrackingAction.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Custom Track Controller and Processing
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "TrackingAction.hh"		// Specifies the file which contains the class structure

#include "OpticalTrajectory.hh"		// Specifies user-defined classes which are called upon in this class
#include "TrackInformation.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "LogSession.hh"
#include "RootIO.hh"

#include "G4Step.hh"			// Specifies all the classes which contain structures called upon in this class
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4TrackVector.hh"
#include "G4TrackingManager.hh"
#include "G4ParticleTypes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VProcess.hh"
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"
#include "G4VVisManager.hh"

#include "G4ios.hh"			// Specifies classes which allow reading/writing into standard input/output
#include <iostream>
#include <fstream>
#include <map>

#include "globals.hh"			// Specifies class defining all global constants and variable types

	// ****** Constructor ****** //
TrackingAction::TrackingAction()
{;}

	// ****** Destructor ****** //
TrackingAction::~TrackingAction()
{;}

	// ****** Pre-Track Processing ****** //
void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
 	// Creates a TrackInformation object to attach to G4Track if track is an optical photon
  if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
  {
    // if (aTrack->GetCurrentStepNumber() ==0){
    if(RootIO::GetInstance()->GetTrack(aTrack->GetParentID())){
      Track* tr =  RootIO::GetInstance()->GetTrack(aTrack->GetParentID());
      tr->tPhot++;
    } //}
    fpTrackingManager->SetTrajectory(new OpticalTrajectory(aTrack));
    fpTrackingManager->SetUserTrackInformation(new TrackInformation());
  }
  else{
    //      if(aTrack->GetParentID()!=0) G4cerr << "Process:\t" << aTrack->GetParentID()<< "\t" << aTrack->GetDefinition()->GetPDGEncoding()<< "\t" <<aTrack->GetCreatorProcess()->GetProcessName()<< G4endl; 
    RootIO::GetInstance()->AddTrack(aTrack->GetTrackID());
    Track* tr =  RootIO::GetInstance()->GetTrack(aTrack->GetTrackID());
    tr->tStep = aTrack->GetCurrentStepNumber();
    if (aTrack->GetCurrentStepNumber() ==0){ // is it first step of particle?
      tr->tParent = aTrack->GetParentID();
      if (aTrack->GetParentID() != 0){      // is it secondary particle?
	tr->tSecondary = 1;  
	tr->tPrimary=0; 
      }
      else{
	tr->tPrimary=1;  
	tr->tSecondary=0; 
      }
      tr->tPhot = 0; 
      tr->tID=aTrack->GetTrackID();
      tr->tTime = aTrack->GetLocalTime();
      tr->tPDGcode = aTrack->GetDefinition()->GetPDGEncoding(); 
      if(aTrack->GetDefinition()->GetPDGEncoding()==1000010030){
	Event* ev = RootIO::GetInstance()->GetEvent();
	if(ev->fNcap==0) {
	  ev->fNcap=3;
	  //  G4cerr << "NCAP 3" << G4endl;
	}
      }
      for(int i=0;i<3;i++){
	tr->tPosi[i] = aTrack->GetPosition()[i]/cm; 
	tr->tP[i] = aTrack->GetMomentum()[i]; 
      }
      tr->tEi = aTrack->GetKineticEnergy();
    }
  }
}

	// ****** Post-Track Processing ****** //
void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
  // If G4Track is an optical photon, check TrackInformation object for flag to draw this track
  if(aTrack->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    {
        OpticalTrajectory* trajectory = (OpticalTrajectory*)(fpTrackingManager->GimmeTrajectory());
      TrackInformation* info = (TrackInformation*)(aTrack->GetUserInformation());
      
      // The track status flag is set by PhotoMultiplierScorer when photon passes quantum efficency of PMT and makes a hit
      if(G4VVisManager::GetConcreteInstance())
	{
	  if(info->GetTrackStatus()) { trajectory->SetDrawTrajectory(true); }
	}
    }
  else{
    if(aTrack->GetTrackStatus()==2){// && aTrack->GetDefinition()->GetPDGCharge()!=0){
      Event* ev = RootIO::GetInstance()->GetEvent();
      Track* tr = RootIO::GetInstance()->GetTrack(aTrack->GetTrackID());
      tr->tStep = aTrack->GetCurrentStepNumber();
      for(int i=0;i<3;i++){
	tr->tPosf[i] = aTrack->GetPosition()[i]/cm; 
      }
      tr->tEf = aTrack->GetKineticEnergy();  
      ev->AddTrack(tr);  
    }  
  }
}

// EOF
