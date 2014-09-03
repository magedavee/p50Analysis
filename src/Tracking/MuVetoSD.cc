#include "MuVetoSD.hh"

#include "RootIO.hh"
#include <G4Step.hh>
#include <G4ios.hh>

MuVetoSD::MuVetoSD(G4String name, G4LogicalVolume* V):
G4VSensitiveDetector(name), myVol(V) {
    RootIO::GetInstance()->addVetoIoniBranch();
}

void MuVetoSD::Initialize(G4HCofThisEvent*) {
    hit_history.clear();
}

G4bool MuVetoSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
    // check that we are in veto proper, not sub-assemblies therein
    G4TouchableHandle hitVol = aStep->GetPreStepPoint()->GetTouchableHandle();
    if(hitVol->GetVolume()->GetLogicalVolume() != myVol) return false;
    
    collectHitInfo(aStep);
    
    IonisationHit* I = ProcessIoniHits(aStep);
    if(!I) return false;
    hit_history.push_back(I);
    return true;
}

bool compare_muioni_times(const IoniCluster& a, const IoniCluster& b) { return a.t < b.t; }

void MuVetoSD::EndOfEvent(G4HCofThisEvent*) {
    vector<IoniCluster> clusts;
    makeClusters(hit_history, clusts);
    std::sort(clusts.begin(), clusts.end(), compare_muioni_times);
    for(auto it = clusts.begin(); it != clusts.end(); it++)  RootIO::GetVetoIoni().AddIoniCluster(*it);
}
