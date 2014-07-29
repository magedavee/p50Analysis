// Unrestricted Use - Property of AECL
//
// CosmicCosineGenerator.cc
// GEANT4 - geant4.9.3.p01
//
// Class File for Surface-Based Cosmic Angular Distribution Generator (for neutron, mu+, mu-)
//	Contains definitions for functions in header file
//
// --------------------------------------------------------
//	Version 1.01 - 2011/04/29 - A. Ho
// --------------------------------------------------------

#include "CosmicCosineGenerator.hh"		// Specifies the file which contains the class structure

#include "LogSession.hh"			// Specifies user-defined classes which are called upon in this class

#include <G4SystemOfUnits.hh>
#include "G4RunManager.hh"			// Specifies all the classes which contain structures called upon in this class
#include "G4PhysicalVolumeStore.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4VSolid.hh"
#include "G4Tubs.hh"
#include "G4VisExtent.hh"
#include "G4UnitsTable.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "Randomize.hh"

#include "G4ios.hh"				// Specifies the class which allows reading/writing to standard input/output

#include "globals.hh"				// Specifies class with variable type definitions and other global parameters
#include "geomdefs.hh"
#include <vector>
#include <math.h>

	// NOTE: Although this file describes a spherical source term, in actuality it is only a hemi-spherical source term!

	// ****** Constructor ****** //
CosmicCosineGenerator::CosmicCosineGenerator(const G4int v, const G4String target)
{
  verbose = v;		// Verbosity (0 = silent, 1 = minimal, 2 = loud)
  if(verbose > 2) { RawData = true; } else { RawData = false; }
  Random = 0.0;

	// Establish initial target volume - defaults to world volume
  worldVolume = G4PhysicalVolumeStore::GetInstance()->at(0);
  targetName = target;
  if(!(targetName == "")) { targetVolume = G4PhysicalVolumeStore::GetInstance()->GetVolume(target); }
  else                    { targetVolume = 0; }
  if(!targetVolume)
  {
    targetVolume = worldVolume;
    targetName = targetVolume->GetName();
    DToWRotation = new G4RotationMatrix(0.,0.,0.);
    DToWTranslation = new G4ThreeVector(0.,0.,0.);
    G4cout << "*** ERROR: Specified target volume not found. Cosine Generator Target defaulted to the world volume, " << targetName << ". ***" << G4endl;
  }
  else
  {
    DToWRotation = FindTargetRotationWRTWorld();
    DToWTranslation = FindTargetTranslationWRTWorld();
    G4cout << "Cosine Generator Target set to " << targetName << "." << G4endl;
  }

	// Initializes source vector container
  PositionMomentum = new std::vector<G4double>();

	// Calculates geometrical values of target volume
  CalculateSourceRadius();
  CalculateTargetSpecs();
}

	// ****** Destructor ****** //
CosmicCosineGenerator::~CosmicCosineGenerator()
{
  delete DToWRotation;
  delete DToWTranslation;
  delete PositionMomentum;
}

	// ****** Change Target Volume ****** //
void CosmicCosineGenerator::SetTargetVolume(G4String target)
{
	// Checks if proposed new target volume exists
  G4String testName = target;
  G4VPhysicalVolume* testVolume = G4PhysicalVolumeStore::GetInstance()->GetVolume(target);
  if(testVolume)
  {
	// If it exists, re-calculate geometrical values of target volume
    targetName = testName;
    targetVolume = testVolume;
    DToWRotation = FindTargetRotationWRTWorld();
    DToWTranslation = FindTargetTranslationWRTWorld();
    CalculateSourceRadius();
    CalculateTargetSpecs();
    if(verbose > 1) { G4cout << "Cosine Generator Target set to " << targetName << "." << G4endl; }
  }
  else
  { 
    G4cout << "*** ATTENTION: Specified target volume not found. Cosine Generator Target kept as " << targetName << ". ***" << G4endl;
  }
}

	// ****** Change Spherical Source Radius ****** //
void CosmicCosineGenerator::SetSourceRadius(G4double r)
{
	// If proposed spherical source radius is negative, reset radius to recommended value
  if(r < 0.) { CalculateSourceRadius(); }
  else if(r < SphereRadius)
  {
	// If proposed spherical source radius is less than recommended, issue a warning in terminal
    if(verbose > 0)
    { G4cout << "*** CAUTION: Setting source sphere radius to a value less than optimal. This may cause inaccurate simulation results. ***" << G4endl; }
    SphereRadius = r;
  }
  else
  {
    SphereRadius = r;
    if(verbose > 1) { G4cout << "Cosine Generator Source Radius set to " << G4BestUnit(SphereRadius,"Length") << "." << G4endl; }
  }
}

	// ****** Change Verbosity ******* //
void CosmicCosineGenerator::SetVerbosity(G4int v)
{
  verbose = v;
  if(verbose > 1) { G4cout << "VolumetricCosineGenerator verbosity set to " << v << "." << G4endl; }
  if(verbose > 2) { RawData = true; G4cout << "*** CAUTION: VolumetricCosineGenerator raw data will now be output. ***" << G4endl; }
  else            { RawData = false; }
}

	// ****** Generate Initial Particle Vector  ****** //
std::vector<G4double>* CosmicCosineGenerator::GenerateSourceLocation(G4bool sub)
{
	// If target volume does not exist, return a null pointer
  if(!targetVolume) { return 0; }

	// Obtain shape of target volume for sampling purposes
  G4VSolid* targetSolid = targetVolume->GetLogicalVolume()->GetSolid();
  G4ThreeVector wNorm, wPosition, wMomentum;
  G4double Prob, nomAng;
  Random = 0.0;
  do
  {
	// Samples a position on the solid surface
    wPosition = GeneratePositionOnDetector(targetSolid);

	// Calculates the normal vector of the sampled surface
    wNorm = (*DToWRotation)*(targetSolid->SurfaceNormal((DToWRotation->inverse())*wPosition));

	// Samples a momentum vector in upper 2pi
    wMomentum = GenerateMomentumObserved();

	// Calculates the angle between proposed momentum and surface normal
    nomAng = std::fabs(wNorm.angle(wMomentum));

	// Acceptance parameter - now governed by GenerateMomentumObserved()
//    Random = G4UniformRand();

	// Calculates the angle between 'up' vector and surface normal
    G4double normAng = std::fabs(wNorm.angle(G4ThreeVector(0.,1.,0.)));
    G4double invAng = 90.*deg - nomAng;

	// Collinear Compounding Factor
//    Prob = cos(nomAng);
    Prob = pow(cos(normAng),2)*cos(nomAng);
    Prob += pow(sin(normAng),2)*cos(nomAng)*(2.00 - exp(0.04*pow(invAng,5)) - 0.040*invAng + 0.043*pow(invAng,5));
  }
  while (Random > Prob || nomAng > 90.*deg);		// Acceptance condition + additional outward directional checking

	// Repositions proposed target location such that the centre of sphere lies on the bottom-most point of target volume
  G4ThreeVector reposition;
  if(pointOne.y() < pointTwo.y()) { reposition.set(0.,pointOne.y(),0.); }
  else				  { reposition.set(0.,pointTwo.y(),0.); }
  wPosition -= reposition;

	// Extends proposed momentum from proposed target location to arrive at proposed source location
  G4ThreeVector sPosition(0.,0.,0.);
  G4double yCheck = CalculateSourceQuadratic(wPosition,wMomentum);
  if(yCheck > SphereRadius) { return GenerateSourceLocation(sub); }
  sPosition.setY(yCheck);
  sPosition.setX(wPosition.x() + (sPosition.y() - wPosition.y())*(wMomentum.x() / wMomentum.y()));
  sPosition.setZ(wPosition.z() + (sPosition.y() - wPosition.y())*(wMomentum.z() / wMomentum.y()));

	// Repositions proposed source location back to match the simulation geometry
  sPosition += reposition;
  sPosition += *DToWTranslation;

	// Assembles proposed source location and proposed momentum into a source vector
  G4double x,y,z, px,py,pz;
  x = sPosition.x();
  y = sPosition.y();
  z = sPosition.z();
  px = -wMomentum.x();
  py = -wMomentum.y();
  pz = -wMomentum.z();
  G4double vector[] = {x, y, z, px, py, pz};
  PositionMomentum->assign(vector,vector+6);
//  G4cout << G4BestUnit((*PositionMomentum)[3],"Length") << G4BestUnit((*PositionMomentum)[4],"Length") << G4BestUnit((*PositionMomentum)[5],"Length") << G4endl;

	// Writes individual momentums into file if requested
  if(!sub && RawData)
  {
    G4ThreeVector pHat(px,py,pz);
    G4double angle = std::fabs(pHat.angle(G4ThreeVector(0.,1.,0.)));
    LogSession* log = LogSession::GetLogSessionPointer();
    log->SetOutputFileName("GeneratedSourceMomenta.txt");
    log->OpenFile(false,true);
    (*log) << pHat.x() << "\t" << pHat.y() << "\t" << pHat.z() << "\t" << angle/deg << std::endl;
    log->CloseFile();
  }

  return PositionMomentum;
}

	// ****** Generate Position On Target Surface ****** //
G4ThreeVector CosmicCosineGenerator::GeneratePositionOnDetector(const G4VSolid* solid)
{
  G4ThreeVector posPropose, normPropose;
  do
  {
	// Proposed position
    posPropose = solid->GetPointOnSurface();

	// Acceptance parameter
    normPropose = solid->SurfaceNormal(posPropose);
    normPropose = (*DToWRotation)*normPropose;
  }
  while (std::fabs(normPropose.angle(G4ThreeVector(0.,1.,0.))) == 180.*deg);	// Acceptance condition

	// Rotate proposed position to physical orientation
  posPropose = (*DToWRotation)*posPropose;
  return posPropose;
}

	// ****** Generate Momentum Direction of Entry into Target ****** //
G4ThreeVector CosmicCosineGenerator::GenerateMomentumObserved()
{
  G4ThreeVector momPropose;
  G4double rPropose, thetaPropose, phiPropose;
  G4double Prob;
//  G4double Random;
  rPropose = 1.*m;
  do
  {
	// Proposed angle
    thetaPropose = 90.*deg*G4UniformRand();

	// Acceptance parameter
    Random = G4UniformRand();

	// Acceptance condition - cosine-squared cosmic angular distribution - now passed out to GenerateSourceLocation() 
    Prob = pow(cos(thetaPropose),2);
  }
  while (Random > Prob);

	// Generates uniform random azimuthal angle
  phiPropose = 360.*G4UniformRand()*deg;

	// Rotate proposed momentum to physical orientation
  momPropose.setRThetaPhi(rPropose,thetaPropose,phiPropose);
  momPropose.rotateX(-90.*deg);
  return momPropose;
}

	// ****** Calculate Target Volume Rotation ****** //
G4RotationMatrix* CosmicCosineGenerator::FindTargetRotationWRTWorld()
{
	// Generate null rotation matrix to start the stack
  G4RotationMatrix* rotation = new G4RotationMatrix(0.,0.,0.);
  G4VPhysicalVolume* currentVolume = worldVolume;

	// Works inward from world volume applying rotations of daughter volumes until it reaches the target volume
  if(targetVolume)
  {
    do
    {
      *rotation *= currentVolume->GetObjectRotationValue();
      G4LogicalVolume* currentLogical = currentVolume->GetLogicalVolume();
      G4int noDaughters = currentLogical->GetNoDaughters();

	// Grabs each daughter volume and checks if target volume is contained within it
      if(noDaughters != 0)
      {
        for(G4int i = 0; i < noDaughters; i++)
        {
          currentVolume = currentLogical->GetDaughter(i);
          if(currentVolume->GetLogicalVolume()->IsAncestor(targetVolume) || currentVolume == targetVolume) break;
        }
      }
      else { currentVolume = targetVolume; }	// Assumes current volume is target volume if it has no daughter volumes
    }
    while (!(currentVolume == targetVolume));
    *rotation *= currentVolume->GetObjectRotationValue();
  }
  return rotation;
}

	// ****** Calculate Target Volume Translation ****** //
G4ThreeVector* CosmicCosineGenerator::FindTargetTranslationWRTWorld()
{
	// Generate null translation vector to start the stack
  G4ThreeVector* translation = new G4ThreeVector(0.,0.,0.);
  G4VPhysicalVolume* currentVolume = worldVolume;

	// Works inward from world volume apply translations of daughter volumes until it reaches the target volume
  if(targetVolume)
  {
    do
    {
      *translation += currentVolume->GetObjectTranslation();
      G4LogicalVolume* currentLogical = currentVolume->GetLogicalVolume();
      G4int noDaughters = currentLogical->GetNoDaughters();

	// Grabs each daughter volume and checks if target volume is contained within it
      if(noDaughters != 0)
      {
        for(G4int i = 0; i < noDaughters; i++)
        {
          currentVolume = currentLogical->GetDaughter(i);
          if(currentVolume->GetLogicalVolume()->IsAncestor(targetVolume) || currentVolume == targetVolume) break;
        }
      }
      else { currentVolume = targetVolume; }	// Assumes current volume is target volume if it has no daughter volumes
    }
    while (!(currentVolume == targetVolume));
    *translation += currentVolume->GetObjectTranslation();
  }
  return translation;
}

	// ****** Calculate Recommended Spherical Source Radius ****** //
void CosmicCosineGenerator::CalculateSourceRadius()
{
	// Obtains target volume shape
  G4VSolid* solid = targetVolume->GetLogicalVolume()->GetSolid();

	// Determines furthest extent of target volume dimensions
  G4double xRange, yRange, zRange;
  if(solid->GetEntityType() == "G4Tubs")
  {
    G4Tubs* tube = dynamic_cast<G4Tubs*>(solid);
    xRange = tube->GetOuterRadius();
    yRange = tube->GetOuterRadius();
    zRange = tube->GetZHalfLength();
  }
  else
  {
    G4VisExtent ranges = solid->GetExtent();
    xRange = ranges.GetXmax();
    yRange = ranges.GetYmax();
    zRange = ranges.GetZmax();
  }

	// Recommended radius is the diagonal edge from corner to opposite corner of volume
  SphereRadius = 2*sqrt(xRange*xRange + yRange*yRange + zRange*zRange);
  if(verbose > 0)
  { G4cout << "Radius of Source Sphere automatically set to " << G4BestUnit(SphereRadius,"Length") << "based on chosen target volume." << G4endl; }
}

	// ****** Calculate Target Volume Size and Dimensions ****** //
void CosmicCosineGenerator::CalculateTargetSpecs()
{
	// Obtain target volume shape
  G4VSolid* solid = targetVolume->GetLogicalVolume()->GetSolid();

	// Determines furthest extent of target volume dimensions
  G4double xRange, yRange, zRange;
  if(solid->GetEntityType() == "G4Tubs")
  {
    G4Tubs* tube = dynamic_cast<G4Tubs*>(solid);
    xRange = tube->GetOuterRadius();
    yRange = tube->GetOuterRadius();
    zRange = tube->GetZHalfLength();
  }
  else
  {
    G4VisExtent ranges = solid->GetExtent();
    xRange = ranges.GetXmax();
    yRange = ranges.GetYmax();
    zRange = ranges.GetZmax();  
  }

	// Permutes the maximum dimensions and rotates them into physical orientation
  G4ThreeVector Permutations[8] = { G4ThreeVector(xRange,yRange,zRange),
				    G4ThreeVector(-xRange,-yRange,-zRange),
				    G4ThreeVector(-xRange,yRange,zRange),
				    G4ThreeVector(xRange,-yRange,zRange),
				    G4ThreeVector(xRange,yRange,-zRange),
				    G4ThreeVector(-xRange,-yRange,zRange),
				    G4ThreeVector(xRange,-yRange,-zRange),
				    G4ThreeVector(-xRange,yRange,-zRange) };
  for(int i = 0; i < 8; i++) { Permutations[i] = (*DToWRotation)*(Permutations[i]); }

	// Determines which permutation represents the highest point and the lowest point for source repositioning
  G4double yMin = 0.0; G4double yMax = 0.0;
  G4int minIndex = -1; G4int maxIndex = -1;
  for(int i = 0; i < 8; i++)
  {
    if((Permutations[i]).y() < yMin) { yMin = (Permutations[i]).y(); minIndex = i; }
    if((Permutations[i]).y() > yMax) { yMax = (Permutations[i]).y(); maxIndex = i; }
  }

	// Stores the final ThreeVectors
  pointOne = Permutations[maxIndex];	// Highest point
  pointTwo = Permutations[minIndex];	// Lowest point
}

	// ****** Line Extension Algorithm ****** //
G4double CosmicCosineGenerator::CalculateSourceQuadratic(G4ThreeVector pos, G4ThreeVector mom)
{
	// Using intersection of lines and planes in space, a quadratic formula was produced
  G4double a,b,c, y;
  a = (pow(mom.x(),2) / pow(mom.y(),2)) + (pow(mom.z(),2) / pow(mom.y(),2)) + 1;
  b = 2*pos.x()*(mom.x() / mom.y()) + 2*pos.z()*(mom.z() / mom.y()) - 2*pos.y()*(a - 1);
  c = pow(pos.x(),2) + pow(pos.z(),2) + pow(pos.y(),2)*(a - 1) - pos.y()*(b + 2*pos.y()*(a - 1)) - pow(SphereRadius,2);

	// Determines if determinant of quadratic formula is valid - if not, the default location is the top of the sphere
  if(pow(b,2) - 4*a*c < 0.) { G4cout << "*** ERROR: Unexpected error in VolumetricCosineGenerator, negative determinant in quadratic formula. ***" << G4endl; y = SphereRadius; }
  else
  {
	// If yes, quadratic formula evaluated - using only the positive outcome guarantees result stays within upper hemisphere of source sphere
    y = (-b + sqrt(pow(b,2) - 4*a*c)) / (2*a);
  }
  return y;
}

// EOF
