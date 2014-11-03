// Unrestricted Use - Property of AECL
// 
// InverseBetaKinematics.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Inverse Beta Decay Reaction Kinematics Generator
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#ifndef InverseBetaKinematics_H		// Only carries out if object is undefined
#define InverseBetaKinematics_H 1	// Defines object

#include "FissionAntiNuModule.hh"	// Specifies user-defined classes which are called upon in this class

#include "G4ThreeVector.hh"		// Specifies the classes which contain structures called upon in this class
#include "G4RotationMatrix.hh"
#include "InverseBetaMessenger.hh"

#include <map>				// Specifies classes defining all global parameters and variable types
#include <vector>
#include "globals.hh"

class G4VPhysicalVolume;		// Structures necessary for class definition

/* -------- Class Definition --------- */

class InverseBetaKinematics
{
  friend class InverseBetaMessenger;

  public:	// Constructors and Destructors

    InverseBetaKinematics(G4int v = 1, const G4String target = "");		// Constructor
    ~InverseBetaKinematics();							// Destructor

  public:	// Accesible Methods

    void SetReactionVolume(G4String);
    void SetVerbosity(G4int);
    void SetAntiNeutrinoDirection(G4ThreeVector dir) { antiNuDir = dir; };
    void SetAntiNeutrinoSpectrum(G4double,G4double,G4double,G4double);
    void SetAntiNeutrinoMonoEnergy(G4double);
    void ToggleFission(G4bool flip) { fMonoEnergy = !flip; };
    void ToggleNeutronGeneration(G4bool);
    void TogglePositronGeneration(G4bool);
    void ToggleSequentialGeneration(G4bool);

    G4String GetReactionVolumeName() { return targetName; };
    G4VPhysicalVolume* GetTargetPhysicalVolume(){ return targetVolume; };
    G4VPhysicalVolume* GetWorldPhysicalVolume(){ return worldVolume; };
    G4ThreeVector GetAntiNeutrinoDirection() const { return antiNuDir; };
    G4double GetSpectrumU235Content(G4bool frac = false) const;
    G4double GetSpectrumU238Content(G4bool frac = false) const;
    G4double GetSpectrumPu239Content(G4bool frac = false) const;
    G4double GetSpectrumPu241Content(G4bool frac = false) const;
    G4bool IsNeutronGenerated() const { return Neutrons; };
    G4bool IsPositronGenerated() const { return Positrons; };
  G4bool IsSequentialGenerated() const { return Sequential; };

    G4ThreeVector GenerateReactionPosition() const;
    vector<G4double>* GenerateReactionKinematics() const;
    void GenerateKinematicsWithoutSimulation(G4int n = 1) const;
    G4double GenerateAntiNeutrinoEnergy() const;

    void PrintAllParameters() const;

  protected:	// Internal Methods

    G4double GeneratePositronAngle(G4double) const;
    G4double CalculatePositronEnergy(G4double,G4double) const;
    G4double CalculateNeutronEnergy(G4double,G4double) const;
    G4double CalculateNeutronAngle(G4double,G4double) const;
    G4double CalculateCrossSectionWRTAngle(G4double,G4double) const;
    G4bool CheckFourVectorSolution(G4double,G4double,G4double,G4double,G4double) const;

    G4RotationMatrix* FindTargetRotationWRTWorld() const;
    G4ThreeVector* FindTargetTranslationWRTWorld() const;

  private:	// Constants

    G4double c;
    G4double pMass, nMass, ePlusMass;

  private:	// Member Data

    G4int verbose;			// Verbosity (0 = silent, 1 = minimal, 2 = loud)
    G4bool RawData;			// Outputs generated numbers, set with verbosity > 2

    G4ThreeVector antiNuDir;
    G4double antiNuMonoEnergy;
    G4bool fMonoEnergy;			// Flag to specify mono-energetic antineutrinos
    G4bool Neutrons;			// Flag to specify neutron production
    G4bool Positrons;			// Flag to specify positron production
    G4bool Sequential;			// Flag to specify sequential positron and neutron production

    FissionAntiNuModule* fission_gen;
    InverseBetaMessenger* inv_messenger;

    G4String targetName;
    G4VPhysicalVolume* targetVolume;
    G4VPhysicalVolume* worldVolume;
    G4RotationMatrix* DToWRotation;
    G4ThreeVector* DToWTranslation;
};

/* ----------------------------------- */

#endif					// End of the if clause

// EOF
