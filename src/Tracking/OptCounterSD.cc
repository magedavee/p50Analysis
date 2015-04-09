#include "OptCounterSD.hh"
#include "Event.hh"
#include "FileIO.hh"

OptCounterSD::OptCounterSD(G4String name): G4VSensitiveDetector(name) {
    FileIO::GetInstance()->addOpticalBranch();
}

G4bool OptCounterSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
    collectHitInfo(aStep);

    G4StepPoint* preStepPoint = aStep->GetPreStepPoint();
    //G4StepPoint* postStepPoint = aStep->GetPostStepPoint();

    ParticleVertex P;
    P.PID = PID;
    
    P.E = preStepPoint->GetKineticEnergy();
    P.t = preStepPoint->GetGlobalTime();
    G4ThreeVector x = worldPrePos;
    for(int i=0; i<3; i++) P.x[i] = x[i];
    G4ThreeVector p = preStepPoint->GetMomentumDirection();
    for(int i=0; i<3; i++) P.p[i] = p[i];
    //printf("P (E=%g, t=%g, x(%g,%g,%g), p(%g,%g,%g), PID=%d\n",P.E, P.t, x[0],P.x[1],P.x[2],P.p[0],P.p[1],P.p[2],P.PID);
    
    FileIO::GetPhoto().AddParticle(P);
    return true;
}
