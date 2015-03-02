#include "StepMax.hh"
#include <G4SystemOfUnits.hh>

StepMax::StepMax(const G4String& processName)
: G4VDiscreteProcess(processName),fMaxChargedStep(1*m) { }

G4bool StepMax::IsApplicable(const G4ParticleDefinition& particle) {
    return (particle.GetPDGCharge() != 0. && !particle.IsShortLived());
}

G4double StepMax::PostStepGetPhysicalInteractionLength( const G4Track&,
                                                        G4double,
                                                        G4ForceCondition* condition )
{
    // condition is set to "Not Forced"
    *condition = NotForced;
    return fMaxChargedStep;
}

G4VParticleChange* StepMax::PostStepDoIt(const G4Track& aTrack, const G4Step&) {
    // do nothing
    aParticleChange.Initialize(aTrack);
    return &aParticleChange;
}
