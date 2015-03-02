#include "TrackingAction.hh"
#include "FileIO.hh"

#include <G4Track.hh>
#include <G4SystemOfUnits.hh>

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack) {
    // record neutron generation
    if(aTrack->GetDefinition()->GetPDGEncoding() == 2112 && aTrack->GetGlobalTime() > 0) {
        NCapt nc;
        nc.t = aTrack->GetGlobalTime();
        nc.E = aTrack->GetKineticEnergy();
        for(int i=0; i<3; i++) nc.x[i] = aTrack->GetPosition()[i];
        nc.capt_A = 1;
        nc.vol = -1000;
        FileIO::GetNCapt().AddNCapt(nc);
    }
    
    // immediate secondaries from long-lived primaries: doesn't work!
    //if(aTrack->GetParentID()==1 && aTrack->GetGlobalTime() > 0.1*s) {
    //    G4Track* tr = (G4Track*)aTrack;
    //    tr->SetGlobalTime(0);
    //}
}
