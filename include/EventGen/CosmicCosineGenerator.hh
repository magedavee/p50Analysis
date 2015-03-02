#ifndef CosmicCosineGenerator_HH
#define CosmicCosineGenerator_HH

#include <vector>

#include <G4ThreeVector.hh>
#include <G4RotationMatrix.hh>
#include <G4VSolid.hh>

class G4VPhysicalVolume;

/// Generates cosmic ray initial position/momentum distribution,
/// from hemisphere directed towards faces of rectangular prism target volume
class CosmicCosineGenerator {
public:
    /// Constructor
    CosmicCosineGenerator(const G4String target = "");
    /// Destructor
    ~CosmicCosineGenerator();

    /// Set target volume by name
    void SetTargetVolume(const G4String);
    /// Set radius of source sphere
    void SetSourceRadius(const G4double);
    
    /// Generate initial particle position/momentum vector
    void GenerateSourceLocation(G4bool sub = false);
    G4ThreeVector pos;  ///< generated position
    G4ThreeVector mom;  ///< generated momentum
    
    G4String GetTargetVolume() const { return targetName; };
    G4double GetSourceRadius() const { return SphereRadius; };
    
protected:
    /// Generate position on target surface
    G4ThreeVector GeneratePositionOnDetector(const G4VSolid*);
    /// Generate momentum direction of entry into target
    G4ThreeVector GenerateMomentumObserved();
    /// determines target volume rotation relative to world volume
    G4RotationMatrix* FindTargetRotationWRTWorld();
    /// determines target volume translation relative to world volume
    G4ThreeVector* FindTargetTranslationWRTWorld();
    /// calculate recommended radius for initial position hemisphere to contain target
    void CalculateSourceRadius();
    /// calculate target size and dimensions
    void CalculateTargetSpecs();
    /// line extension algorithm
    G4double CalculateSourceQuadratic(G4ThreeVector x, G4ThreeVector p);
    
    G4double SphereRadius;              ///< source sphere radius
    G4double Random;
    
    G4String targetName;                ///< name of target volume
    G4VPhysicalVolume* worldVolume;     ///< world volume
    G4VPhysicalVolume* targetVolume;    ///< target volume
    G4RotationMatrix* DToWRotation;     ///< target rotation to world
    G4ThreeVector* DToWTranslation;     ///< target translation to world
    G4ThreeVector targTop;              ///< high point on target
    G4ThreeVector targBottom;           ///< low point on target
    
};

#endif
