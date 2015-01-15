#include "TrackingAction.hh"
#include "RootIO.hh"

#include <G4Track.hh>

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
    if(aTrack->GetDefinition()->GetPDGEncoding() == 2112 && aTrack->GetGlobalTime() > 0) {
            NCapt nc;
            nc.t = aTrack->GetGlobalTime();
            nc.E = aTrack->GetKineticEnergy();
            for(int i=0; i<3; i++) nc.x[i] = aTrack->GetPosition()[i];
            nc.capt_A = 1;
            nc.vol = -1000;
            RootIO::GetNCapt().AddNCapt(nc);
    }
}
