#include "IoniSD.hh"
#include "RootIO.hh"

#include <G4SystemOfUnits.hh>
#include <G4UnitsTable.hh>
#include <G4Step.hh>
#include <G4Track.hh>
#include <G4TouchableHandle.hh>

////////////////////////////////////////////////////////////////

void IonisationHit::Display() const {
    G4cout << "Hit E=" << G4BestUnit(sum_w,"Energy") 
    << " at t=" << G4BestUnit(GetTime(),"Time") << "( " << G4BestUnit(GetDTime(),"Time")
    << ")\tx=[ " << G4BestUnit(GetPos(),"Length") << "] { " << G4BestUnit(GetDPos(),"Length") << "}" << G4endl;
}

IoniCluster hitToCluster(IonisationHit* h, G4int pid, G4int detvol) {
    IoniCluster c;
    c.E = h->GetEnergyDeposit();
    c.t = h->GetTime();
    c.dt = h->GetDTime();
    c.PID = pid;
    for(unsigned int i=0; i<3; i++) { c.x[i] = h->GetPos()[i]; c.dx[i] = h->GetDPos()[i]; }
    c.vol = detvol;
    return c;
}

bool compare_hit_times(const IonisationHit* a, const IonisationHit* b) { return a->GetTime() < b->GetTime(); }

////////////////////////////////////////////////////////////////

IoniSD::IoniSD(): time_gap(50*ns), edep_threshold(10*keV) { }

void IoniSD::collectHitInfo(G4Step* aStep) {
    G4TouchableHandle hitVol = aStep->GetPreStepPoint()->GetTouchableHandle();
    worldPos = aStep->GetPreStepPoint()->GetPosition();
    localPos = hitVol->GetHistory()->GetTopTransform().TransformPoint(worldPos);
    PID = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
}

IonisationHit* IoniSD::ProcessIoniHits(G4Step* aStep) {
    G4double E = aStep->GetTotalEnergyDeposit()-aStep->GetNonIonizingEnergyDeposit();
    if(!E) return NULL;
    
    IonisationHit* aHit = new IonisationHit();
    
    aHit->SetEnergy(E);
    aHit->SetLength(aStep->GetStepLength());
    aHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime()+0.5*aStep->GetDeltaTime());
    aHit->SetPos(localPos);
    aHit->record();
    return aHit;
}

void IoniSD::makeClusters(vector<IonisationHit*>& hits, vector<IoniCluster>& clusts) {
    if(!hits.size()) return;
    
    // time-order hit events
    std::sort(hits.begin(), hits.end(), compare_hit_times);
    
    // group into timing clusters
    auto ihit = hits.begin();
    IonisationHit* prevHit = *ihit;
    ihit++;
    for(; ihit != hits.end(); ihit++) {
        if((*ihit)->GetTime() > prevHit->GetTime() + time_gap) {
            if(prevHit->GetEnergyDeposit() > edep_threshold) clusts.push_back(hitToCluster(prevHit, PID, seg_id));
            else delete prevHit;
            prevHit = *ihit;
        } else {
            *prevHit += **ihit;
            delete *ihit;
        }
    }
    if(prevHit->GetEnergyDeposit() > edep_threshold) clusts.push_back(hitToCluster(prevHit, PID, seg_id));
    else delete prevHit;
}
