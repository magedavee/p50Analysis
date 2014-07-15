// Unrestricted Use - Property of AECL
// 
// CosmicNeutronGenerator.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Cosmic Neutron Energy Spectrum Generator
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef CosmicNeutronGenerator_H		// Only carries out if object is undefined
#define CosmicNeutronGenerator_H 1		// Defines object

#include <map>					// Specifies classes defining all global parameters and variable types
#include <vector>
#include "globals.hh"
#include "CosmicNeutronMessenger.hh"

class CosmicCosineGenerator;			// Structures necessary for class definition

/* -------- Class Definition --------- */

class CosmicNeutronGenerator
{
  friend class CosmicNeutronMessenger;

  public:	// Constructors and Destructors

    CosmicNeutronGenerator(G4int v = 1, const G4String target = "");		// Constructor
    CosmicNeutronGenerator(G4double,G4double,G4double,G4double,G4int v = 1);	// Overloaded Constructor - initialize with s, rc, d, w
    CosmicNeutronGenerator(G4double,G4int v = 1);				// Overloaded Constructor - initialize with nMono

    ~CosmicNeutronGenerator();			// Destructor

  public:	// Accessible Methods

    void SetVerbosity(G4int);
    void SetSolarModulation(G4double);
    void SetCutoffRigidity(G4double);
    void SetAtmosphericDepth(G4double, G4bool IsKm = false);
    void SetWaterEquivalence(G4double);
    void SetMinEnergy(G4double);
    void SetMaxEnergy(G4double);
    void SetMonoEnergyFlag(G4bool flip) { fMonoEnergy = flip; };
    void SetNeutronMonoEnergy(G4double);
    void SetTargetVolume(G4String);
    void SetSourceRadius(G4double);

    G4double GetSolarModulation() const { return s; };
    G4double GetCutoffRigidity() const { return rc; };
    G4double GetAtmosphericDepth() const { return d; };
    G4double GetWaterEquivalence() const { return w; };
    G4double GetMinEnergy() const { return min_e; };
    G4bool GetMonoEnergyFlag() const { return fMonoEnergy; };
    G4double GetNeutronMonoEnergy() const { return nMono; };
    G4double GetMaxEnergy() const { return max_e; };
    G4String GetTargetVolume() const;
    G4double GetSourceRadius() const;
    CosmicCosineGenerator* GetCosineGenerator() { return cos_gen; };

    G4double CalculateLethargyValue(const G4double) const;
    G4double CalculateFluxValue(const G4double) const;
    void GenerateEnergiesWithoutSimulation(const G4int n = 1) const;
    std::vector<G4double>* GenerateSourceLocation() const;
    G4double GenerateNeutronEnergy() const;
    G4double GenerateNeutronEnergyWithCDF() const;	// For a quicker, less accurate simulation - not quite working yet!!!

    void PrintAllParameters();

  protected:	// Internal Methods

    void GenerateCDFTable();
    G4double ConvertToGramPerSqCm(const G4double) const;
    G4double ConvertToKm(const G4double) const;
    void CalculateMaximumFluxValue();			// This method is coded such that it works for this specific spectrum ONLY!!! Not guaranteed to work with other spectra.
    G4double CalculateLowEnergyFluxTerm() const;
    G4double CalculateBasicSpectrumTerm(const G4double) const;
    G4double CalculateGroundEffectTerm(const G4double) const;
    G4double CalculateThermalSpectrumTerm(const G4double) const;

  private:	// User-defined Variables

    G4double s;		// Solar Modulation
    G4double rc;	// Cut-off Rigidity
    G4double d;		// Depth in Atmosphere
    G4double w;		// Water-equivalent Content

    G4double nMono;		// Neutron Mono-Energy
    G4double min_e, max_e;	// Minimum and Maximum Energy to Sample
    G4double max_dist;		// Maximum Flux Value to make iterator a bit more efficient

    std::map<G4double,G4double>* theCDF;

    G4int nCDFSplits;
    G4bool fUpdateTable;
    G4bool fMonoEnergy;

    G4int verbose;		// Verbosity (0 = silent, 1 = minimal, 2 = loud)
    G4bool RawData;		// Outputs generated numbers, set with verbosity > 2

  private:	// Constants

    G4double s_min, s_max;	// Solar Minimum and Solar Maximum values, set in constructors

  private:	// Member Data

    CosmicCosineGenerator* cos_gen;
    CosmicNeutronMessenger* neutron_messenger;
};

/* ----------------------------------- */

#endif						// End of the if clause

// EOF
