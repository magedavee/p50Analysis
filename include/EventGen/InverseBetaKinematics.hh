#ifndef InverseBetaKinematics_H
#define InverseBetaKinematics_H

#include "InverseBetaMessenger.hh"
#include <vector>
using std::vector;

/// Kinematics generator for inverse beta decay
class InverseBetaKinematics {
friend class InverseBetaMessenger;
    
public:
    /// Constructor, with verbosity
    InverseBetaKinematics(G4int v = 1);
    
    /// Set verbosity
    void SetVerbosity(G4int);
    /// Set fuel composition determining spectrum
    void SetAntiNeutrinoSpectrum(G4double, G4double, G4double, G4double);
    /// Set monoenergetic antineutrino energy (set =0 to use spectrum)
    void SetAntiNeutrinoMonoEnergy(G4double);
    /// Set whether to use Dwyer spectrum
    void SetDwyer(G4bool val) { dwyer = val; }
    
    /// generate reaction kinematics vector
    vector<double> GenerateReactionKinematics() const;
    /// Spectrum Test Function
    void GenerateKinematicsWithoutSimulation(G4int n = 1) const;
    /// calculate antinu energy
    G4double GenerateAntiNeutrinoEnergy() const;
    /// calculate antinu energy from Dwyer spectrum
    G4double GenerateDwyerAntiNeutrinoEnergy() const;
    
    /// display summary of parameter settings
    void PrintAllParameters() const;
    
    G4double GetUranium235Content(G4bool frac = false) const { return frac? CalculateFractionComposition(U235) : U235; }
    G4double GetUranium238Content(G4bool frac = false) const { return frac? CalculateFractionComposition(U238) : U238; }
    G4double GetPlutonium239Content(G4bool frac = false) const { return frac? CalculateFractionComposition(Pu239) : Pu239; }
    G4double GetPlutonium241Content(G4bool frac = false) const { return frac? CalculateFractionComposition(Pu241) : Pu241; }
    
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
    
    G4double Dwyer235[10200];   ///< Dwyer U235 spectrum table
    G4double Dwyer238[10200];   ///< Dwyer U238 spectrum table
    G4double Dwyer239[10200];   ///< Dwyer Pu239 spectrum table
    G4double Dwyer241[10200];   ///< Dwyer Pu241 spectrum table
    G4bool dwyer;               ///< whether to use Dwyer spectrum
    
    G4double U235, U238, Pu239, Pu241;          ///< Fuel compositions
    InverseBetaMessenger* fiss_messenger;       ///< UI messenger
    
    /// calculate positron emission angle
    G4double GeneratePositronAngle(G4double) const;
    /// calculate positron total energy
    G4double CalculatePositronEnergy(G4double,G4double) const;
    /// calculate neutron total energy
    G4double CalculateNeutronEnergy(G4double,G4double) const;
    /// calculate neutron emission angle
    G4double CalculateNeutronAngle(G4double,G4double) const;
    G4double CalculateCrossSectionWRTAngle(G4double,G4double) const;
    G4bool CheckFourVectorSolution(G4double,G4double,G4double,G4double,G4double) const;
        
    G4double c;
    G4double pMass, nMass, ePlusMass;
    
private:
    
    G4int verbose;                              ///< Verbosity (0 = silent, 1 = minimal, 2 = loud)
    G4bool RawData;                             ///< Outputs generated numbers, set with verbosity > 2
    G4double antiNuMonoEnergy;                  ///< monoenergetic neutrino energy to generate (set =0 for spectrum)
    InverseBetaMessenger inv_messenger;         ///< messenger for this class
};

#endif
