// Unrestricted Use - Property of AECL
//
// SteppingAction.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Custom Step Controller and Processing
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "SteppingAction.hh"		// Specifies the file which contains the class structure

#include "RunAction.hh"			// Specifies user-defined classes which are called upon in this class
#include "TrackInformation.hh"
#include "LogSession.hh"
#include "RootIO.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"			// Specifies all the classes which contain structures called upon in this class
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TrackVector.hh"
#include "G4ParticleTypes.hh"
#include "G4RunManager.hh"
#include "G4TrackStatus.hh"

#include "G4ios.hh"			// Specifies classes which allow reading/writing into standard input/output
#include <iostream>
#include <fstream>
#include <map>

#include "globals.hh"			// Specifies class defining all global constants and variable types

	// ****** Constructor ****** //
SteppingAction::SteppingAction()
{;}

	// ****** Destructor ****** //
SteppingAction::~SteppingAction()
{;}

	// ****** Post-Step Processing ****** //
void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
  RunAction* run_action = (RunAction*)(G4RunManager::GetRunManager()->GetUserRunAction());
  if(run_action->GetRecordLevel() == 2){ 
    if(aStep->GetTrack()->GetParentID()==0 && (aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetName() == "LeadShield" || aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetName() == "PolyShieldB" ||aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetName() == "PolyShieldLi")){
      RootIO::GetInstance()->SetFill(1);
    }
  }
  DetectorConstruction* detector = (DetectorConstruction*)(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  if(aStep->GetPostStepPoint()->GetStepStatus()!=1){
    if (aStep->GetTrack()->GetDynamicParticle()->GetCharge()!=0){ 
      if(aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetName()==detector->GetScintLog(0,0)->GetName() || aStep->GetPostStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetName()==detector->GetScintLog(0,0)->GetName()){

	RootIO::GetInstance()->GetTrack(aStep->GetTrack()->GetTrackID())->tEdep += aStep->GetTotalEnergyDeposit();
	RootIO::GetInstance()->GetEvent()->fEdep += aStep->GetTotalEnergyDeposit();
	G4int cono = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetCopyNo();
	RootIO::GetInstance()->GetEvent()->fSegEdep[cono] += aStep->GetTotalEnergyDeposit();

	//track level segment energy depositions
	int count = 0, found = 0;
		while(count<RootIO::GetInstance()->GetTrack(aStep->GetTrack()->GetTrackID())->tSegHit && found == 0){	
	  if(cono == RootIO::GetInstance()->GetTrack(aStep->GetTrack()->GetTrackID())->tSegment[count]){
	    found = 1;
	    RootIO::GetInstance()->GetTrack(aStep->GetTrack()->GetTrackID())->tSegEdep[count] += aStep->GetTotalEnergyDeposit();
	  }
	  count++;
	  }
	if(found == 0){
	  // new segment
	  RootIO::GetInstance()->GetTrack(aStep->GetTrack()->GetTrackID())->tSegHit++;
	  RootIO::GetInstance()->GetTrack(aStep->GetTrack()->GetTrackID())->tSegment.push_back(cono);
	  RootIO::GetInstance()->GetTrack(aStep->GetTrack()->GetTrackID())->tSegEdep.push_back(aStep->GetTotalEnergyDeposit());
	}

	if(run_action->GetRecordLevel() == 1) RootIO::GetInstance()->SetFill(1);
      }
    }
  }

  G4OpBoundaryProcessStatus boundaryStatus=Undefined;
  static G4OpBoundaryProcess* boundary=NULL;
  
  //find the boundary process only once
  if(!boundary){
    G4ProcessManager* pm 
      = aStep->GetTrack()->GetDefinition()->GetProcessManager();
    G4int nprocesses = pm->GetProcessListLength();
    G4ProcessVector* pv = pm->GetProcessList();
    G4int i;
    for( i=0;i<nprocesses;i++){
      if((*pv)[i]->GetProcessName()=="OpBoundary"){
	boundary = (G4OpBoundaryProcess*)(*pv)[i];
	break;
      }
    }
  }
  G4ParticleDefinition* particleType = aStep->GetTrack()->GetDefinition();
  
  /*  if(particleType==G4OpticalPhoton::OpticalPhotonDefinition()){
    boundaryStatus=boundary->GetStatus();
    if(aStep->GetPostStepPoint()->GetStepStatus()==fGeomBoundary){
      // switch(boundaryStatus){
      // case TotalInternalReflection: G4cerr << "TIR:\t" << boundary->GetIncidentAngle()<< G4endl;
      //   if(boundaryStatus==TotalInternalReflection){
	//	G4cerr << "TIR:\t" << boundary->GetIncidentAngle()*180/3.149<< G4endl;
      //  }
    }
    }*/
	// Additional processing that can occur at the end of each step
	/*
       // Tracking muon decay and Michel spectra
  if(aStep->GetTrack()->GetDefinition() == G4Positron::PositronDefinition())
  {
    if(aStep->GetPostStepPoint()->GetProcessDefinedStep() != 0 && aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "annihil")
    {
      G4double angle = std::fabs((aStep->GetTrack()->GetVertexMomentumDirection()).angle(G4ThreeVector(0.,1.,0.)));
      LogSession* log = LogSession::GetLogSessionPointer();
      log->SetOutputFileName("MichelRawData.txt");
      log->OpenFile(false,true);
      (*log) << (aStep->GetTrack()->GetVertexKineticEnergy()/MeV) << "\t" << (angle/deg) << std::endl;
      log->CloseFile();
    }
  }
*/
/*
	// Tracking optical photon generation distribution
  if(aStep->GetTrack()->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
  {
    G4double energy = (aStep->GetTrack()->GetVertexKineticEnergy()/eV);
    G4double lambda = 1.23984e+3/energy;
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName("ScintillatorLambdaData.txt");
    log->OpenFile(false,true);
    (*log) << lambda << std::endl;
    log->CloseFile();
    aStep->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);
  }
*/
 
}

// EOF
