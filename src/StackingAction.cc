#include "StackingAction.hh"

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* aTrack) {
    if(noTracks) return fKill;
    return G4UserStackingAction::ClassifyNewTrack(aTrack);
}
