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
    c.l = h->GetLength();
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
    worldPrePos = aStep->GetPreStepPoint()->GetPosition(); // track occurred in this volume
    worldPostPos = aStep->GetPostStepPoint()->GetPosition();
    localPrePos = hitVol->GetHistory()->GetTopTransform().TransformPoint(worldPrePos);
    localPostPos = hitVol->GetHistory()->GetTopTransform().TransformPoint(worldPostPos);
    localMidPos = (localPrePos + localPostPos)*0.5;
    PID = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
}

IonisationHit* IoniSD::ProcessIoniHits(G4Step* aStep) {
    G4double E = aStep->GetTotalEnergyDeposit()-aStep->GetNonIonizingEnergyDeposit();
    if(E <= 0) return NULL;
    
    IonisationHit* aHit = new IonisationHit();
    
    // split up track segment to produce spread information on single-segment tracks
    const unsigned int nsplit = 3;
    for(unsigned int i=0; i<nsplit; i++) {
        double l = (i+0.5)/double(nsplit);
        aHit->SetEnergy(E/nsplit);
        aHit->SetLength(aStep->GetStepLength()/nsplit);
        aHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime()+l*aStep->GetDeltaTime());
        aHit->SetPos(localPrePos*(1.-l)+localPostPos*l);
        aHit->record();
    }
    
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
