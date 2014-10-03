#include "ScintSD.hh"

#include "RootIO.hh"
#include "RunAction.hh"
#include "ScintTankBuilder.hh"

#include <cassert>
#include <vector>

#include <G4SystemOfUnits.hh>
#include <G4Step.hh>
#include <G4Track.hh>
#include <G4VProcess.hh>
#include <G4TouchableHandle.hh>
#include <G4Neutron.hh>
#include <G4Gamma.hh>
#include <G4RunManager.hh>
#include <G4UnitsTable.hh>
#include <G4ProcessType.hh>
#include <G4HadronicProcessType.hh>
#include <G4ios.hh>

ScintSD::ScintSD(G4String name, ScintSegVol& T):
G4VSensitiveDetector(name), verbose(0), myScint(T) {
    time_gap = 1*ns;
    RootIO::GetInstance()->addScIoniBranch();
    RootIO::GetInstance()->addNCaptBranch();
}

void ScintSD::Initialize(G4HCofThisEvent*) {
    verbose = G4RunManager::GetRunManager()->GetVerboseLevel();
    hit_history.clear();
    secondaries_counter.clear();
}

G4bool ScintSD::ProcessHits(G4Step* aStep, G4TouchableHistory* H) {
    
    collectHitInfo(aStep);
    
    G4TouchableHandle hitVol = aStep->GetPreStepPoint()->GetTouchableHandle();
    if(hitVol->GetVolume()->GetLogicalVolume() != myScint.scint_log) seg_id = -1000000; // special segment ID for "dead" material
    else seg_id = myScint.getSegmentNum(localPrePos);
    
    G4bool notable = ProcessNeutronHits(aStep, H);
    IonisationHit* I = ProcessIoniHits(aStep);
    notable |= (I != NULL);
    if(I)  hit_history[std::pair<G4int,G4int>(PID,seg_id)].push_back(I);
    secondaries_counter[aStep->GetTrack()->GetTrackID()] = aStep->GetSecondary()->size();
    
    return notable;
}

G4bool ScintSD::ProcessNeutronHits(G4Step* aStep, G4TouchableHistory*) {
    // Check that this is a neutron
    if(aStep->GetTrack()->GetDefinition() != G4Neutron::NeutronDefinition()) return false;
    
    // Check that this is a hadronic capture (or special case for 6Li inelastic scattering)
    const G4VProcess* PDS = aStep->GetPostStepPoint()->GetProcessDefinedStep();
    bool isCapt = PDS->GetProcessSubType() == fCapture;
    if(!(PDS && PDS->GetProcessType() == fHadronic && (isCapt || PDS->GetProcessSubType() == fHadronInelastic))) return false;
    
    if(verbose >= 2) PDS->DumpInfo();
    
    NCapt nc;
    for(unsigned int i=0; i<3; i++) nc.x[i] = localPrePos[i];
    nc.t = aStep->GetPostStepPoint()->GetGlobalTime();
    nc.vol = seg_id;
    nc.E = aStep->GetPreStepPoint()->GetKineticEnergy();
    
    // tally secondaries; check for recoiling nucleus
    nc.Ngamma = 0;
    nc.Egamma = 0;
    nc.capt_Z = nc.capt_A = 0;
    nc.Nprod = 0;
    const G4TrackVector* secondaries = aStep->GetSecondary();
    assert(secondaries);
    G4TrackVector::const_iterator it = secondaries->begin() + secondaries_counter[aStep->GetTrack()->GetTrackID()];
    for(; it != secondaries->end(); it++) {
        nc.Nprod++;
        G4ParticleDefinition* pt = (*it)->GetDefinition();
        assert(pt);
        if(pt == G4Gamma::GammaDefinition()) {
            nc.Egamma += (*it)->GetKineticEnergy();
            nc.Ngamma++;
        } else {
            G4int Z = pt->GetAtomicNumber();
            G4int A = pt->GetAtomicMass();
            if(verbose >= 2) {
                G4cout << "\tFragment: " << pt->GetParticleType() << "/" << pt->GetParticleSubType()
                << " (" << Z << "," << A << ") " << G4BestUnit((*it)->GetKineticEnergy(),"Energy") << G4endl;
            }
            nc.capt_Z += Z;
            nc.capt_A += A;
        }
    }
    
    if(isCapt || (nc.capt_Z == 3 && nc.capt_A == 7)) {
        RootIO::GetNCapt().AddNCapt(nc);
    
        if(verbose >= 2) {
            G4cout << "Neutron ( KE=" << G4BestUnit(nc.E,"Energy") << ") capture at [ " << G4BestUnit(localPrePos,"Length") << "] with "
                << nc.Ngamma << " gammas ( E=" << G4BestUnit(nc.Egamma,"Energy")
                << ") of " << nc.Nprod << " final products; total (A,Z)=(" << nc.capt_A << "," << nc.capt_Z << ")" << G4endl;
        }
    
        return true;
        
    } else return false;
}

bool compare_ioni_times(const IoniCluster& a, const IoniCluster& b) { return a.t < b.t; }

void ScintSD::EndOfEvent(G4HCofThisEvent*) {
    
    if(verbose >= 2 && hit_history.size())
        G4cout << "Processing ionization hits in " << hit_history.size() << " volumes." << G4endl;
    
    vector<IoniCluster> clusts;
    for(auto it = hit_history.begin(); it != hit_history.end(); it++) {
        PID = it->first.first;
        seg_id = it->first.second;
        makeClusters(it->second, clusts);
    }
    std::sort(clusts.begin(), clusts.end(), compare_ioni_times);
    for(auto it = clusts.begin(); it != clusts.end(); it++)  RootIO::GetScIoni().AddIoniCluster(*it);
}
