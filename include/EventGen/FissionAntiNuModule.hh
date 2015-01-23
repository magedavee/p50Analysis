#ifndef FISSIONANTINUMODULE_HH
/// Assure headers are only loaded once
#define FISSIONANTINUMODULE_HH

#include "PrimaryGeneratorAction.hh"

class FissionAntiNuMessenger;

/// Inverse Beta Decay event generator module
class FissionAntiNuModule: public PrimaryGeneratorModule {
public:
    /// Constructor
    FissionAntiNuModule(PrimaryGeneratorAction* P);
    /// Destructor
    ~FissionAntiNuModule();
    
    /// throw event particles
    virtual void GeneratePrimaries(G4Event* anEvent);

    G4double GetUranium235Content(G4bool frac = false) const { return frac? CalculateFractionComposition(U235) : U235; }
    G4double GetUranium238Content(G4bool frac = false) const { return frac? CalculateFractionComposition(U238) : U238; }
    G4double GetPlutonium239Content(G4bool frac = false) const { return frac? CalculateFractionComposition(Pu239) : Pu239; }
    G4double GetPlutonium241Content(G4bool frac = false) const { return frac? CalculateFractionComposition(Pu241) : Pu241; }
    
    /// Set fuel composition
    void SetAntiNeutrinoSpectrum(G4double,G4double,G4double,G4double);
    
    G4double GenerateAntiNeutrinoEnergy() const;
  G4double GenerateDwyerAntiNeutrinoEnergy() const;
    void GenerateEnergiesWithoutSimulation(G4int n = 1) const;
    
    void PrintAllParameters() const;
    void SetDwyer(G4bool);
protected:

    /// Calculate Contribution from Uranium-235
    G4double CalculateU235Spectrum(G4double) const;
    /// Calculate Contribution from Plutonium-239
    G4double CalculatePu239Spectrum(G4double) const;
    /// Calculate Contribution from Plutonium-241
    G4double CalculatePu241Spectrum(G4double) const;
    /// Calculate Contribution from Uranium-238
    G4double CalculateU238Spectrum(G4double) const;
    /// Calculate Fraction of Total Composition
    G4double CalculateFractionComposition(G4double x) const { return x / (U235 + U238 + Pu239 + Pu241); }

  G4double Dwyer235[10200];
  G4double Dwyer238[10200];
  G4double Dwyer239[10200];
  G4double Dwyer241[10200];
  G4bool dwyer;
    
    G4double U235, U238, Pu239, Pu241;          ///< Fuel compositions
    FissionAntiNuMessenger* fiss_messenger;     ///< UI messenger
};
 
#endif
