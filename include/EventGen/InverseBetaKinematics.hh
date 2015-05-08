/// \file InverseBetaKinematics.hh \brief Calculator for reactor antineutrino spectrum and IBD kinematics.
#ifndef InverseBetaKinematics_H
#define InverseBetaKinematics_H

#include "InverseBetaMessenger.hh"
#include "DwyerLangford.hh"
#include <vector>
using std::vector;

/// specification for inverse beta decay kinematics
struct IBDKinematics {
    double Ee;                  ///< positron kinetic energy
    double En;                  ///< neutron kinetic energy
    double Enu;                 ///< antineutrino kinetic energy
    double theta_e;             ///< positron angle to neutrino
    double theta_n;             ///< neutron angle to neutrino
    G4ThreeVector phat_e;       ///< positron momentum direction
    G4ThreeVector phat_n;       ///< neutron momentum direction
    G4ThreeVector phat_nu;      ///< antineutrino momentum direction
};

/// Kinematics generator for inverse beta decay
class InverseBetaKinematics {
public:
    /// Constructor, with verbosity
    InverseBetaKinematics(G4int v = 1);
    
    /// Set verbosity
    void SetVerbosity(G4int v);
    /// Set fuel composition determining spectrum
    void SetAntiNeutrinoSpectrum(G4double U5, G4double U8, G4double P3, G4double P4);
    /// Set monoenergetic antineutrino energy (set =0 to use spectrum)
    void SetAntiNeutrinoMonoEnergy(G4double Enu);
    /// Set whether to use Dwyer spectrum
    void SetDwyer(G4bool val) { dwyer = val; }
    
    /// generate reaction kinematics
    IBDKinematics GenerateReactionKinematics() const;
    /// Spectrum Test Function
    void GenerateKinematicsWithoutSimulation(G4int n = 1) const;
    /// calculate antinu energy
    G4double GenerateAntiNeutrinoEnergy() const;
    /// calculate antinu energy from Dwyer spectrum
    G4double GenerateDwyerAntiNeutrinoEnergy() const;
    
    /// display summary of parameter settings
    void PrintAllParameters() const;
    
    /// portion of fuel which is U235
    G4double GetUranium235Content(G4bool frac = false) const { return frac? CalculateFractionComposition(U235) : U235; }
    /// portion of fuel which is U238
    G4double GetUranium238Content(G4bool frac = false) const { return frac? CalculateFractionComposition(U238) : U238; }
    /// portion of fuel which is Pu239
    G4double GetPlutonium239Content(G4bool frac = false) const { return frac? CalculateFractionComposition(Pu239) : Pu239; }
    /// portion of fuel which is Pu241
    G4double GetPlutonium241Content(G4bool frac = false) const { return frac? CalculateFractionComposition(Pu241) : Pu241; }
    
protected:
    
    /// Calculate Contribution from Uranium-235
    G4double CalculateU235Spectrum(G4double eNu) const;
    /// Calculate Contribution from Plutonium-239
    G4double CalculatePu239Spectrum(G4double eNu) const;
    /// Calculate Contribution from Plutonium-241
    G4double CalculatePu241Spectrum(G4double eNu) const;
    /// Calculate Contribution from Uranium-238
    G4double CalculateU238Spectrum(G4double eNu) const;
    /// Calculate Fraction of Total Composition
    G4double CalculateFractionComposition(G4double x) const { return x / (U235 + U238 + Pu239 + Pu241); }
   
    G4bool dwyer;                               ///< whether to use Dwyer spectrum
    
    G4double U235, U238, Pu239, Pu241;          ///< Fuel compositions
    
    /// calculate positron emission angle
    G4double GeneratePositronAngle(G4double eNu) const;
    /// calculate positron total energy
    G4double CalculatePositronEnergy(G4double ePlusAngle, G4double nuEnergy) const;
    /// calculate neutron total energy
    G4double CalculateNeutronEnergy(G4double ePlusEnergy, G4double nuEnergy) const;
    /// calculate neutron emission angle
    G4double CalculateNeutronAngle(G4double nEnergy, G4double nuEnergy) const;
    /// calculate interaction cross section
    G4double CalculateCrossSectionWRTAngle(G4double angle, G4double nuEnergy) const;
    /// Verify solution is kinematically allowed
    G4bool CheckFourVectorSolution(G4double nuEnergy, G4double posE, G4double nE, G4double posAng, G4double nAng) const;
        
    const G4double pMass, nMass, ePlusMass;     ///< particle masses
    G4int verbose;                              ///< Verbosity (0 = silent, 1 = minimal, 2 = loud)
    G4bool RawData;                             ///< Outputs generated numbers, set with verbosity > 2
    G4double antiNuMonoEnergy;                  ///< monoenergetic neutrino energy to generate (set =0 for spectrum)
    G4ThreeVector nuDirection;                  ///< incident neutrino direction
    InverseBetaMessenger inv_messenger;         ///< messenger for this class
    DwyerLangford DL;
};

#endif
