// Unrestricted Use - Property of AECL
// 
// FissionAntiNuGenerator.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Fission Antineutrino Energy Spectrum Generator
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef FissionAntiNuGenerator_H	// Only carries out if object is undefined
#define FissionAntiNuGenerator_H 1	// Defines object

#include "G4ThreeVector.hh"		// Specifies the classes which contain structures called upon in this class

#include <map>				// Specifies classes defining all global parameters and variable types
#include <vector>
#include "globals.hh"
#include "FissionAntiNuMessenger.hh"

/* -------- Class Definition --------- */

class FissionAntiNuGenerator
{
  friend class FissionAntiNuMessenger;

  public:	// Constructors and Destructors

    FissionAntiNuGenerator(G4bool M = true,G4int v = 1);	// Constructor
    ~FissionAntiNuGenerator();					// Destructor

  public:	// Accessible Methods

    void SetAntiNeutrinoSpectrum(G4double,G4double,G4double,G4double);		// Set fuel composition
    void SetVerbosity(G4int);

    G4double GetUranium235Content(G4bool frac = false) const;
    G4double GetUranium238Content(G4bool frac = false) const;
    G4double GetPlutonium239Content(G4bool frac = false) const;
    G4double GetPlutonium241Content(G4bool frac = false) const;

    G4double GenerateAntiNeutrinoEnergy() const;
    void GenerateEnergiesWithoutSimulation(G4int n = 1) const;

    void PrintAllParameters() const;

  private:	// Internal Methods

    G4double CalculateFractionComposition(G4double) const;
    G4double CalculateU235Spectrum(G4double) const;
    G4double CalculatePu239Spectrum(G4double) const;
    G4double CalculatePu241Spectrum(G4double) const;
    G4double CalculateU238Spectrum(G4double) const;

  private:	// Member Data

    G4double U235, U238, Pu239, Pu241;			// Fuel compositions
    FissionAntiNuMessenger* fiss_messenger;

    G4int verbose;					// Verbosity (0 = silent, 1 = minimal, 2 = loud)
    G4bool RawData;					// Outputs generated numbers, set with verbosity > 2
};

/* ----------------------------------- */

#endif					// End of the if clause

// EOF
