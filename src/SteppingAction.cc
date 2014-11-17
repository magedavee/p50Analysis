#include "SteppingAction.hh"
#include "EventAction.hh"

#include <G4SteppingManager.hh>
#include <G4String.hh>
#include <G4EventManager.hh>
#include <G4Event.hh>

void SteppingAction::UserSteppingAction(const G4Step* aStep) {
        // check that computation limit is not exceeded (trapped events)
        timeSpentSoFar = ((EventAction*)G4EventManager::GetEventManager()->GetUserEventAction())->getCPUTime();
        if(timeSpentSoFar > 10) {
                G4cout << "Tracking killed by computation time limit" << G4endl;
                aStep->GetTrack()->SetTrackStatus(fStopAndKill);
                isTrapped = true;
        }
}
