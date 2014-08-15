#include "FluxCounterSD.hh"
#include "Event.hh"
#include "RootIO.hh"

FluxCounterSD::FluxCounterSD(G4String name): G4VSensitiveDetector(name) {
    RootIO::GetInstance()->addFluxBranch();
}

G4bool FluxCounterSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
        
    G4StepPoint* p0 = aStep->GetPreStepPoint();
    G4StepPoint* p1 = aStep->GetPostStepPoint();

    //if(!aStep->IsFirstStepInVolume()) return false; // entering particles only
    if(p0->GetPhysicalVolume() == p1->GetPhysicalVolume()) return false;
    
    EventPrimaryPtcl P;
    P.PID = aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
    if(record_PIDs.size() && !record_PIDs.count(P.PID)) return false; // only particles on list
    
    P.E = p0->GetKineticEnergy();
    P.t = p0->GetGlobalTime();
    G4ThreeVector x = p0->GetPosition();
    for(int i=0; i<3; i++) P.x[i] = x[i];
    G4ThreeVector p = p0->GetMomentumDirection();
    for(int i=0; i<3; i++) P.p[i] = p[i];
        
    RootIO::GetFlux().AddParticle(P);
    return true;
}
