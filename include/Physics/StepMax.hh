/// \file StepMax.hh \brief Step-size limiting physics process.
#ifndef STEPMAX_HH
#define STEPMAX_HH

#include <globals.hh>
#include <G4VDiscreteProcess.hh>
#include <G4ParticleDefinition.hh>
#include <G4Step.hh>

/// Stepsize limiter process
class StepMax : public G4VDiscreteProcess {
public:
    /// Constructor
    StepMax(const G4String& processName = "UserMaxStep");
    /// check which particles this applies to
    virtual G4bool IsApplicable(const G4ParticleDefinition&);
    /// set maximum step length
    void SetMaxStep(G4double step) { fMaxChargedStep = step; }
    /// get maximum step length
    G4double GetMaxStep() { return fMaxChargedStep; }
    /// get length-limited step
    virtual G4double 
    PostStepGetPhysicalInteractionLength(const G4Track& track,
                                         G4double previousStepSize,
                                         G4ForceCondition* condition);
    /// apply (non)effects to particle
    virtual G4VParticleChange* PostStepDoIt(const G4Track&, const G4Step&);
    /// get mean free path
    virtual G4double GetMeanFreePath(const G4Track&,G4double,G4ForceCondition*) {return DBL_MAX;}  
    
private:
    G4double fMaxChargedStep;   ///< maximum step size
};

#endif
