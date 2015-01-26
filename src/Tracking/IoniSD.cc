#include "IoniSD.hh"
#include "RootIO.hh"

#include <cassert>

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
    c.EdEdx = h->GetEdEdx();
    c.EdEdx2 = h->GetEdEdx2();
    c.PID = pid;
    for(unsigned int i=0; i<3; i++) { c.x[i] = h->GetPos()[i]; c.dx[i] = h->GetDPos()[i]; }
    c.vol = detvol;
    return c;
}

bool compare_hit_times(const IonisationHit* a, const IonisationHit* b) { return a->GetTime() < b->GetTime(); }

////////////////////////////////////////////////////////////////

IoniSD::IoniSD(): time_gap(50*ns), edep_threshold(10*keV) { }

void IoniSD::collectHitInfo(G4Step* aStep) {
    worldPrePos = aStep->GetPreStepPoint()->GetPosition(); // track occurred in this volume
    worldPostPos = aStep->GetPostStepPoint()->GetPosition();
    localPrePos = W2S.coordPtoC(worldPrePos);
    localPostPos = W2S.coordPtoC(worldPostPos);
    localMidPos = (localPrePos + localPostPos)*0.5;
    PID = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
}

IonisationHit* IoniSD::ProcessIoniHits(G4Step* aStep) {
    G4double Etot = aStep->GetTotalEnergyDeposit();
    G4double E = Etot-aStep->GetNonIonizingEnergyDeposit();
    G4double z = aStep->GetTrack()->GetDynamicParticle()->GetCharge();
    
    // dE/dx estimation
    G4double dEdx = 0;
    auto pdedx = parent_dEdx.find(aStep->GetTrack());
    bool isSubQuench = pdedx != parent_dEdx.end();
    if(isSubQuench) {
        dEdx = pdedx->second;
        parent_dEdx.erase(pdedx);
    }
        
    if(!E) return NULL;
    
    IonisationHit* aHit = new IonisationHit();
    
    // split up track segment to produce spread information on single-segment tracks;
    // using Simpson's Rule weights w for numerical integration of quantities
    const unsigned int nsplit = 7;
    for(unsigned int i=0; i<nsplit; i++) {
        double w = i%2? 4 : 2;
        if(i==0 || i==nsplit-1) w = 1;
        w /= 3*(nsplit-1);
        double l = i/double(nsplit-1);
        aHit->SetEnergy(E*w);
        aHit->SetLength(aStep->GetStepLength()*w);
        aHit->SetTime(aStep->GetPreStepPoint()->GetGlobalTime()+l*aStep->GetDeltaTime());
        aHit->SetPos(localPrePos*(1.-l)+localPostPos*l);
        
        // dE/dx estimation
        if(!isSubQuench) {
            G4double m_x = aStep->GetTrack()->GetDynamicParticle()->GetMass();
            G4double KE = aStep->GetTrack()->GetKineticEnergy() + l*Etot; // kinetic energy before step
            if(!m_x) { m_x = 0.511*MeV; KE = E; z = -1; } // treat massless particles like electrons
            G4double x = KE/m_x;
            if(x>9.59e-5) {
                G4double b2 = 1-1/pow(1+x,2); // particle beta^2
                dEdx = 0.307 * MeV/cm * mat_n * z*z / b2 * (log(1.022e4*b2/(1-b2)) - b2);
            } else {
                dEdx = mat_n * z*z * 57017*pow(x,0.4290) * MeV/cm;
            }
        }
        assert(dEdx==dEdx); // NaN check
        aHit->SetdEdx(dEdx);
        
        aHit->record();
    }
    
    // save dE/dx for secondaries
    const G4TrackVector* secondaries = aStep->GetSecondary();
    for(auto it = secondaries->begin(); it != secondaries->end(); it++) {
        if((*it)->GetVolume() != aStep->GetTrack()->GetVolume()) continue;
        parent_dEdx.insert(std::pair<const G4Track*,double>(*it,aHit->GetEdEdx()/aHit->GetEnergyDeposit()));
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
