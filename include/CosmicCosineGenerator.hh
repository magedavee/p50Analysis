// Unrestricted Use - Property of AECL
// 
// CosmicCosineGenerator.hh
// GEANT4 - geant4.9.3.p01
//
// Header File for Surface-Based Cosmic Angular Distribution Generator (for neutron, mu+, mu-)
//	Contains class functions/variables
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
//      Edited 2014/07 M. P. Mendenhall
// --------------------------------------------------------

#ifndef CosmicCosineGenerator_H		// Only carries out if object is undefined
#define CosmicCosineGenerator_H 1		// Defines object

#include "G4ThreeVector.hh"			// Specifies the classes which contain structures called upon in this class
#include "G4RotationMatrix.hh"
#include "G4VSolid.hh"

#include <vector>				// Specifies classes defining all global parameters and variable types
#include "globals.hh"

class G4VPhysicalVolume;			// Structures necessary for class definition

/* -------- Class Definition --------- */

/// Generates cosmic ray initial position/momentum distribution,
/// from hemisphere directed towards faces of rectangular prism target volume
class CosmicCosineGenerator
{
  public:	// Constructors and Destructors

    CosmicCosineGenerator(const G4int v = 0, const G4String target = "");			// Constructor
    ~CosmicCosineGenerator();									// Destructor

  public:	// Accessible Methods

    void SetTargetVolume(const G4String);			// Set methods
    void SetSourceRadius(const G4double);
    void SetVerbosity(const G4int);
    std::vector<G4double>* GenerateSourceLocation(G4bool sub = false);

    G4String GetTargetVolume() const { return targetName; };	// Get methods
    G4double GetSourceRadius() const { return SphereRadius; };

  protected:	// Internal Methods

    G4ThreeVector GeneratePositionOnDetector(const G4VSolid*);
    G4ThreeVector GenerateMomentumObserved();
    /// determines target volume rotation relative to world volume
    G4RotationMatrix* FindTargetRotationWRTWorld();
    /// determines target volume translation relative to world volume
    G4ThreeVector* FindTargetTranslationWRTWorld();
    /// calculates appropriate radius for initial position hemisphere to contain target
    void CalculateSourceRadius();
    void CalculateTargetSpecs();
    G4double CalculateSourceQuadratic(G4ThreeVector,G4ThreeVector);

  private:	// Member Data

    G4double SphereRadius;
    std::vector<G4double>* PositionMomentum;
    G4double Random;

//    G4ThreeVector upDir;
    G4String targetName;
    G4VPhysicalVolume* worldVolume;
    G4VPhysicalVolume* targetVolume;
    G4RotationMatrix* DToWRotation;
    G4ThreeVector* DToWTranslation;
    G4ThreeVector pointOne;
    G4ThreeVector pointTwo;

    G4int verbose;     ///< Verbosity (0 = silent, 1 = minimal, 2 = loud)
    G4bool RawData;    ///< Outputs generated numbers, set with verbosity > 2
};

/* ----------------------------------- */

#endif						// End of the if clause

// EOF
