#include "SteppingAction.hh"
#include "EventAction.hh"

#include <G4SteppingManager.hh>
#include <G4String.hh>
#include <G4EventManager.hh>
#include <G4Event.hh>

SteppingAction::SteppingAction (): calcTimeCmd("/run/maxCalcTime", this) {
    
    calcTimeCmd.SetGuidance("Set maximum time to calculate one event [s]");
    calcTimeCmd.SetDefaultValue(maxCalcTime);
    
    Reset();
}

void SteppingAction::SetNewValue(G4UIcommand* command, G4String newValue) {
    if(command==&calcTimeCmd) {
        maxCalcTime = calcTimeCmd.GetNewDoubleValue(newValue);
        printf("Setting maximum calculation time to %g s.\n", maxCalcTime);
    }
}

void SteppingAction::CheckBoundaryStatus(G4OpBoundaryProcessStatus boundaryStatus) {
    G4String outputstring;
    switch ( boundaryStatus ) {
        case 0:
            outputstring="Undefined";
            break;
        case 1:
            outputstring="FresnelRefraction";
            break;
        case 2:
            outputstring="FresnelReflection";
            break;
        case 3:
            outputstring="TotalInternalReflection";
            break;
        case 4:
            outputstring="LambertianReflection";
            break;
        case 5:
            outputstring="LobeReflection";
            break;
        case 6:
            outputstring="SpikeReflection";
            break;
        case 7:
            outputstring="backscattering";
            break;
        case 8:
            outputstring="Absorption";
            break;
        case 9:
            outputstring="Detection";
            break;
        case 10:
            outputstring="NotAtBoundary";
            break;
        case 11:
            outputstring="SameMaterial";
            break;
        case 12:
            outputstring="StepTooSmall";
            break;
        case 13:
            outputstring="NoRINDEX";
            break;
        default:
            outputstring="Undefined";
            break;
    }
    printf("BoundaryStatus=%s\n",outputstring.data());
}


void SteppingAction::UserSteppingAction(const G4Step* aStep) {
    // check that computation limit is not exceeded (trapped events)
    timeSpentSoFar = ((EventAction*)G4EventManager::GetEventManager()->GetUserEventAction())->getCPUTime();
    if(timeSpentSoFar > maxCalcTime) {
        if(!isTrapped) G4cout << "Tracking killed by computation time limit" << G4endl;
        aStep->GetTrack()->SetTrackStatus(fStopAndKill);
        isTrapped = true;
    }
    
    if(!debugOptical) return;
    
    G4Track* theTrack=aStep->GetTrack();
    G4int theTrackID=theTrack->GetTrackID();
    G4int theTrackParentID=theTrack->GetParentID();
    //G4StepPoint* thePrePoint = aStep->GetPreStepPoint();
    //G4StepPoint* thePostPoint = aStep->GetPostStepPoint();
    const G4TrackVector* theSecondary=aStep->GetSecondary();
    G4int sec_size=theSecondary->size(); //size for secondaries particles
    if ( theTrack->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition() ) {
        G4OpBoundaryProcessStatus boundaryStatus = Undefined;
        G4OpBoundaryProcess* boundary = NULL;
        if( !boundary ) {
            G4ProcessManager* pm = theTrack->GetDefinition()->GetProcessManager();
            G4int nprocesses = pm->GetProcessListLength();
            G4ProcessVector* pv = pm->GetProcessList();
            boundary=(G4OpBoundaryProcess*)(*pv)[0];
            for (G4int j=0; j<nprocesses; j++ ) {
                if( (*pv)[j]->GetProcessName()=="OpBoundary" ) {
                    boundary=(G4OpBoundaryProcess*)(*pv)[j];
                    break;
                }
            }
        }
        boundaryStatus = boundary->GetStatus();
        printf("theTrackID=%d, theTrackParentID=%d, particlename=%s, have %d secondaries, ",theTrackID,theTrackParentID,theTrack->GetDefinition()->GetParticleName().data(),sec_size);
        CheckBoundaryStatus(boundaryStatus);
        G4String Logical_Volume_Name="NULL";
        if ( theTrack->GetVolume() ) {
            Logical_Volume_Name=theTrack->GetVolume()->GetLogicalVolume()->GetName();
        }
        G4String Next_Logical_Volume_Name="NULL";
        if ( theTrack->GetNextVolume() ) {
            Next_Logical_Volume_Name=theTrack->GetNextVolume()->GetLogicalVolume()->GetName();
        }
        boundaryStatus = boundary->GetStatus();
        printf("volume=%s,next_volume=%s\n",Logical_Volume_Name.data(),Next_Logical_Volume_Name.data());
    }
    /////////////////////
}
