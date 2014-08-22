#ifndef CosmicMuonGenerator_H
#define CosmicMuonGenerator_H

#include "CosmicCosineGenerator.hh"
#include "CosmicMuonMessenger.hh"

#include <map>

class CosmicMuonGenerator: public CosmicCosineGenerator {
  friend class CosmicMuonMessenger;

  public:

    CosmicMuonGenerator(const G4String target = "");    // Constructor
    CosmicMuonGenerator(G4bool,G4double PlusMinus = 0.0);	// Overloaded Constructor - initialize with BESS spectrum and +/- ratio
    CosmicMuonGenerator(G4double,G4double PlusMinus = 1.0);	// Overloaded Constructor - initialize with muMono

    ~CosmicMuonGenerator();

    void SetMinRange(G4double);
    void SetMaxRange(G4double);
    void SetMonoEnergyFlag(G4bool flip) { fMonoEnergy = flip; };
    void SetMuonMonoEnergy(G4double);
    void SetPlusMinusRatio(G4double);
    void SetMuonPlusOnlyFlag(G4bool);
    void SetMuonMinusOnlyFlag(G4bool);
    void SetBESSSpectrumFlag(G4bool);
    void SetTestAngle(G4double);

    G4double GetMinMomentum() const { return min_val; };
    G4double GetMaxMomentum() const { return max_val; };
    G4double GetMuonMonoEnergy() const { return muMono; };
    G4double GetPlusMinusRatio() const { return ratio; };
    G4bool GetMuonPlusFlag() const { return fPlusOnly; };
    G4bool GetMuonMinusFlag() const { return fMinusOnly; };
    G4bool GetBESSSpectrumFlag() const { return fBESS; };
    G4ThreeVector GetTestDirection() const { return testDir; };

    void GenerateEnergiesWithoutSimulation(const G4int n = 1) const;
    G4double GenerateMuonEnergy(G4ThreeVector muDir = G4ThreeVector(0.,1.,0.)) const;
    G4int GenerateMuonType(G4double,G4ThreeVector muDir = G4ThreeVector(0.,1.,0.)) const;

    void PrintAllParameters() const;

  protected:	// Internal Methods
    
    /// set default values for parameters
    void setDefaults();
    void GenerateLipariPDFTable();
    void GenerateBESSPDFTable();

    void GenerateLipariEnergiesWithoutSimulation(const G4int n = 1) const;
    void GenerateBESSEnergiesWithoutSimulation(const G4int n = 1) const;
    G4double GenerateLipariMuonEnergy(G4ThreeVector) const;
    G4double GenerateBESSMuonEnergy() const;
    G4int GenerateLipariMuonType(G4double,G4ThreeVector) const;
    G4int GenerateBESSMuonType(G4double) const;

  private:	// User-defined Variables

    G4double min_val, max_val;	// Minimum and maximum particle momentum to sample
    G4double ratio;		// Ratio of muon plus to muon minus generated
    G4ThreeVector testDir;	// Incident muon zenith angle for test function
    G4double muMono;		// Specified mono-energy

    G4bool fMonoEnergy;		// Flag to set mono-energetic muons
    G4bool fPlusOnly;		// Flag to set 100% mu+ generation
    G4bool fMinusOnly;		// Flag to set 100% mu- generation
    G4bool fBESS;		// Flag to set BESS spectrum

    G4int verbose;		// Verbosity (0 = silent, 1 = minimal, 2 = loud)
    G4bool RawData;		// Outputs generated numbers, set with verbosity > 2

  private:	// Member Data

    CosmicMuonMessenger* muon_messenger;

    G4double minBESS_p, maxBESS_p;		// Hard limits for BESS spectrum
    G4double minLipari_e, maxLipari_e;		// Hard limits for Lipari spectrum
    G4double muMass;				// Muon mass in MeV/c

    G4double BinRangeMax[48];			// For generation of BESS spectrum
    G4double BinRangeMin[48];
    G4double BinRangeAvg[48];
    G4double BinAcceptanceCutoff[2][48];
    G4double BinSlope[2][48];

    G4double Ang[8];				// For generation of Lipari spectrum
    G4double En[13];
    G4double Flx[13][8];
    G4double Ratio[13][8];
    G4double ZAng[13][8];
};


#endif
